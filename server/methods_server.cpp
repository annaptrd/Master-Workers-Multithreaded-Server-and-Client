
#include "methods.h"
#include "methods_server.h"
#include "CountriesList.h"
#include "ChildParameters.h"
#include <iostream>
#include <sys/types.h>      /* sockets */
#include <sys/socket.h>      /* sockets */
#include <netinet/in.h>      /* internet sockets */
#include <netdb.h>          /* gethostbyaddr */
#include <sstream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
#include <fstream>
#include <fcntl.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <stdlib.h>

using namespace std;


static ServerStructures serverStructures;

ServerStructures * getServerStructures() {
    return &serverStructures;
}

void listCountriesForAggregator() {
    serverStructures.countries->printListDetails();
}

void initializeSockets(int queryPort, int statsPort) { //arxikopoiisi twn pipes
    serverStructures.server_for_clients.sin_family = AF_INET;
    serverStructures.server_for_clients.sin_addr.s_addr = htonl(INADDR_ANY);
    serverStructures.server_for_clients.sin_port = htons(queryPort);

    serverStructures.server_for_workers.sin_family = AF_INET;
    serverStructures.server_for_workers.sin_addr.s_addr = htonl(INADDR_ANY);
    serverStructures.server_for_workers.sin_port = htons(statsPort);

    if ((serverStructures.socket_for_clients = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    if ((serverStructures.socket_for_workers = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

  

    if (bind(serverStructures.socket_for_clients, (struct sockaddr *) &serverStructures.server_for_clients, sizeof (serverStructures.server_for_clients)) < 0) {
        perror("bind");
        exit(1);
    }

    if (bind(serverStructures.socket_for_workers, (struct sockaddr *) &serverStructures.server_for_workers, sizeof (serverStructures.server_for_workers)) < 0) {
        perror("bind");
        exit(1);
    }


    if (listen(serverStructures.socket_for_clients, 500) < 0) {
        perror("listen");
        exit(1);
    }

    if (listen(serverStructures.socket_for_workers, 500) < 0) {
        perror("listen");
        exit(1);
    }

    cout << "Sockets created successfully at " << queryPort << " and " << statsPort << endl;
}

void cleanupSockets() { //gia tin katastrofi tous
    shutdown(serverStructures.socket_for_clients, SHUT_RDWR);
    shutdown(serverStructures.socket_for_workers, SHUT_RDWR);

    close(serverStructures.socket_for_clients);
    close(serverStructures.socket_for_workers);

    cout << "Sockets closed successfully" << endl;
}

void initializeWorkers(int bufferSize) { //arxikopoiisi workers
    serverStructures.tids = new pthread_t[serverStructures.w];
    serverStructures.bufferSize = bufferSize;

    int * ready = new int;
    *ready = 0;

    printf("-------------------------------------------\n");

    pthread_mutex_init(&serverStructures.m, NULL);

    for (int i = 0; i < serverStructures.threads; i++) {
        ChildParameters * parameters = new ChildParameters(); //apothikeuw tis parametrous gia ta pipes stis domes
        parameters->i = i;
        parameters->bufferSize = 5000;
        parameters->m_pointer = &serverStructures.m;
        parameters->queue = serverStructures.queue;

        pthread_create(&serverStructures.tids[i], NULL, main_worker, (void*) parameters);
    }


}

void cleanupWorkers() { //stelnei sigkill stous workers

    for (int i = 0; i < serverStructures.threads; i++) {
        serverStructures.queue->place(new QueueItem(NULL));
    }
    for (int i = 0; i < serverStructures.threads; i++) {
        pthread_join(serverStructures.tids[i], NULL);
    }

    delete [] serverStructures.tids;

    pthread_mutex_destroy(&serverStructures.m);
}

int connectTo(string ip, int port) {
    int sock;
    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*) &server;
    struct hostent *rem;

    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        cout << "cannot connect to " << ip << " " << port << endl;
        exit(1);
    }

    if ((rem = gethostbyname(ip.c_str())) == NULL) {
        herror("gethostbyname");
        cout << "cannot connect to " << ip << " " << port << endl;
        exit(1);
    }

    server.sin_family = AF_INET; /* Internet domain */
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(port); /* Server port */

    if (connect(sock, serverptr, sizeof (server)) < 0) {
        perror("connect");
        cout << "cannot connect to " << ip << " " << port << endl;
        exit(1);
    } else {
        return sock;
    }
}

int * broadcastToOne(int port, string ip, string msg) { //to xrisimopoiw stis periptwseis tis select, wste na paroun oi workers tin entoli
   
    int * fds = new int[1];
    char * packet = new char[serverStructures.bufferSize]();

    strcpy(packet, msg.c_str()); //vazei sto packet tin entoli

    fds[0] = connectTo(ip, port);
    write_all(fds[0], packet, serverStructures.bufferSize);
    delete [] packet;

    return fds;
}

int * broadcast(WorkersData* sworkers, string msg) { //to xrisimopoiw stis periptwseis tis select, wste na paroun oi workers tin entoli
    int i = 0;
    int size = sworkers->getSize();
    int * fds = new int[size]();

    char * packet = new char[serverStructures.bufferSize]();

    strcpy(packet, msg.c_str()); //vazei sto packet tin entoli

    WorkerNode * node = sworkers->head;


    while (node != NULL) {
        fds[i++] = connectTo(node->worker->ip, node->worker->port); //syndesi me workers
        node = node->next;
    }

    for (int i = 0; i < size; i++) {
        write_all(fds[i], packet, serverStructures.bufferSize); //stelnei tin entoli se olous tous workers
    }
    delete [] packet;

    return fds;
}

int * waitForConnection() {
    struct sockaddr_in client;
    socklen_t clientlen = sizeof (client);

    fd_set active_fd_set;

    FD_ZERO(&active_fd_set);

    FD_SET(serverStructures.socket_for_clients, &active_fd_set);
    FD_SET(serverStructures.socket_for_workers, &active_fd_set);


    if (select(FD_SETSIZE, &active_fd_set, NULL, NULL, NULL) < 0) {
        cout << "Select error !!!  " << endl;
        return NULL;
    }

    if (FD_ISSET(serverStructures.socket_for_clients, &active_fd_set)) {
        cout << "Client connected !!!  " << endl;

        int fd = accept(serverStructures.socket_for_clients, (struct sockaddr *) &client, &clientlen);

        int * conndata = new int[2];
        conndata[0] = fd;
        conndata[1] = 0;

        return conndata;
    }


    if (FD_ISSET(serverStructures.socket_for_workers, &active_fd_set)) {
        cout << "Master connected !!!  " << endl;

        int fd = accept(serverStructures.socket_for_workers, (struct sockaddr *) &client, &clientlen);

        int * conndata = new int[2];
        conndata[0] = fd;
        conndata[1] = 1;

        return conndata;
    }

    return NULL;
}


PacketsList::PacketsList() {
    head = NULL;
}

PacketsList::~PacketsList() {
    PacketNode* current;
    PacketNode* previous;
    previous = head;
    current = head;
    while (current != NULL) {
        previous = current;
        current = current->next;
        delete previous;
    }
}

void PacketsList::insertNode(char* p) //eisagwgi neou komvou sto telos
{
    pthread_mutex_lock(&serverStructures.m);
    PacketNode* newnode = new PacketNode;
    newnode->next = NULL;
    newnode->packet = p;

    if (head == NULL) {
        head = newnode;
    } else {
        PacketNode* current = head;
        while ((current->next) != NULL) {
            current = (current->next);
        }
        current->next = newnode;
    }
    pthread_mutex_unlock(&serverStructures.m);
}


int write_all(int fd, const void *buff, int size) {
    const char * buff2 = (const char *)buff;
    int sent, n;
    for (sent = 0; sent < size; sent += n) {
        if ((n = write(fd, buff2 + sent, size - sent)) == -1) {
            return -1; /* error */
        } else if (n == 0) {
            break;
        }
    }
    return sent;
}

int read_all(int fd, void *buff, int size) {
    char * buff2 = (char *)buff;
    int sent, n;
    for (sent = 0; sent < size; sent += n) {
        if ((n = read(fd, buff2 + sent, size - sent)) == -1) {
            return -1; /* error */
        } else if (n == 0) {
            break;
        }
    }
    return sent;
}



char* collectPatientRecordWithSelect(int * fds, int size) { //omoiws
    serverStructures.w = size;

    char * packet = new char[serverStructures.bufferSize]();
    fd_set active_fd_set;

    bool * answers_received = new bool[serverStructures.w];

    for (int i = 0; i < serverStructures.w; i++) {
        answers_received[i] = false;
    }

    int answers_missing = serverStructures.w;

    bool found = false;
    while (answers_missing > 0 && found==false) {
        FD_ZERO(&active_fd_set);

        for (int i = 0; i < serverStructures.w; i++) {
            if (!answers_received[i]) {
                FD_SET(fds[i], &active_fd_set);
            }
        }

        if (select(FD_SETSIZE, &active_fd_set, NULL, NULL, NULL) < 0) {
            cout << "Select error !!!  " << endl;
            return NULL;
        }

        for (int i = 0; i < serverStructures.w; i++) {
            if (FD_ISSET(fds[i], &active_fd_set)) {
                if (read_all(fds[i], packet, serverStructures.bufferSize) <= 0) { //pairnei tin apantisi
                    break;
                }
                if ((strcmp(packet, "###") != 0)) {
                    cout << packet << endl;
                    found = true;
                    break;
                }
                answers_received[i] = true;
                answers_missing--;
            }
        }
    }
    if (found == false)
        cout << "Patient id not found!" << endl;
    printf("Select finished ! \n");
    delete [] answers_received;

    return packet;
}

int collectDiseaseFrequencyWithSelect(int * fds, int size) { //omoiws
    serverStructures.w = size;

    char * packet = new char[serverStructures.bufferSize]();
    fd_set active_fd_set;

    bool * answers_received = new bool[serverStructures.w];

    for (int i = 0; i < serverStructures.w; i++) {
        answers_received[i] = false;
    }

    int answers_missing = serverStructures.w;

    int sum = 0;

    while (answers_missing > 0) {
        FD_ZERO(&active_fd_set);

        for (int i = 0; i < serverStructures.w; i++) {
            if (!answers_received[i]) {
                FD_SET(fds[i], &active_fd_set);
            }
        }

        if (select(FD_SETSIZE, &active_fd_set, NULL, NULL, NULL) < 0) {
            cout << "Select error !!!  " << endl;
            return -1;
        }

        for (int i = 0; i < serverStructures.w; i++) {
            if (FD_ISSET(fds[i], &active_fd_set)) {
                if (read_all(fds[i], packet, serverStructures.bufferSize) <= 0) { //pairnei tin apantisi
                    break;
                }

                sum = sum + atoi(packet);

                answers_received[i] = true;
                answers_missing--;
            }
        }
    }

    printf("Select finished ! \n");

    cout << sum << endl;

    delete [] packet;

    delete [] answers_received;

    return sum;
}
