#include "methods.h"
#include "methods_client.h"
#include "CommandsList.h"
#include "ChildParameters.h"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <sys/types.h>      /* sockets */
#include <sys/socket.h>      /* sockets */
#include <netinet/in.h>      /* internet sockets */
#include <unistd.h>          /* read, write, close */
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
#include <netdb.h>

using namespace std;


static ClientStructures clientStructures;

ClientStructures * getAggregatorStructures() {
    return &clientStructures;
}

void initializeCommands(string queryFile) {

    clientStructures.commandsList = new CommandsLinkedList();
    clientStructures.queryFile = queryFile;
    
    ifstream recordsfile(queryFile);

    if (recordsfile.is_open() && recordsfile.good()) {
        string line = "";
        int l = 0;
        while (getline(recordsfile, line)) {
            if (line != "" && line != "\r" && line != "\n") { //se periptwsi kenwn, na t agnoei
                stringstream command(line);
               
                Command* c = new Command(line);
                clientStructures.commandsList->insertNode(c);
                
                l++;
            }
        }

    } else {
        cout << "Failed to open file..";
    }

    printf("--------------- Client Aggregator -----------------\n");
    clientStructures.commandsList->printListDetails();
    printf("-------------------------- -----------------\n");
}

CommandsLinkedList * getCommandsAgg() {
    return clientStructures.commandsList;
}

void assignCommands() {//anathetei entoles stous workers
    CommandsNode* current = clientStructures.commandsList->head;
    int i = 0;
    while (current != NULL) {
        current->command->worker_id = i; 
        cout << current->command->query << " " << current->command->worker_id << endl;
        current = current->next;


        i = i + 1;
        if (i >= clientStructures.w) {
            i = 0;
        }
    }
}


void initializeW(int w, string ip, int port) { //arxikopoiisi w, ip, port k mutexes
    clientStructures.w = w;
    clientStructures.ip = ip;
    clientStructures.port = port;

    pthread_mutex_init(&clientStructures.m, NULL);
    pthread_cond_init(&clientStructures.c, NULL);
}


void initializeWorkers(int bufferSize) { //arxikopoiisi workers
    clientStructures.tids = new pthread_t[clientStructures.w];
    clientStructures.bufferSize = bufferSize;

    int * ready = new int;
    *ready = 0;

    printf("-------------------------------------------\n");

    for (int i = 0; i < clientStructures.w; i++) {
        ChildParameters * parameters = new ChildParameters(); //apothikeuw tis parametrous gia ta pipes stis domes
        parameters->i = i;
        parameters->bufferSize = clientStructures.bufferSize;
        parameters->commandsList = clientStructures.commandsList;
        parameters->ip = clientStructures.ip;
        parameters->port = clientStructures.port;
        parameters->w = clientStructures.w;
        parameters->w_ready_pointer = ready;
        parameters->m_pointer = &clientStructures.m;
        parameters->c_pointer = &clientStructures.c;

        pthread_create(&clientStructures.tids[i], NULL, &main_worker, (void*) parameters);
    }
}

void cleanupWorkers() { 
    for (int i = 0; i < clientStructures.w; i++) {
        pthread_join(clientStructures.tids[i], NULL);
    }

    pthread_mutex_destroy(&clientStructures.m);
    pthread_cond_destroy(&clientStructures.c);

    delete [] clientStructures.tids;
}


int connectTo(string ip, int port) { //gia ti syndesi
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

void sendToServer(int fd, string msg, int bufferSize) { //apostoli sto server
    char * packet = new char[bufferSize]();

    strcpy(packet, msg.c_str());

    write_all(fd, packet, bufferSize);

    delete [] packet;
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

char * receiveFromServer(int fd, int bufferSize) { //gia ti disease frequency 
    char * packet = new char[bufferSize]();
    int sum = 0;

    if (read_all(fd, packet, bufferSize) <= 0) { //pairnei tin apantisi
        return NULL;
    }
    sum = sum + atoi(packet);

    return packet;
}
