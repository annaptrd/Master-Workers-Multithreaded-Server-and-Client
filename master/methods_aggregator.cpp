
#include "methods.h"
#include "methods_aggregator.h"
#include "CountriesList.h"
#include "ChildParameters.h"
#include <iostream>
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


static AggregatorStructures aggregatorStructures;

AggregatorStructures * getAggregatorStructures() {
    return &aggregatorStructures;
}

void aggrTotal() {
    aggregatorStructures.total++;
}

void aggrErrors() {
    aggregatorStructures.errors++;
}

void initializeCountries(string inputdir) { //xwres sti lista
    DIR *dir_ptr; //directory 
    struct dirent *direntp;

    aggregatorStructures.countries = new CountriesLinkedList();
    aggregatorStructures.inputdir = inputdir;

    if ((dir_ptr = opendir(inputdir.c_str())) == NULL) {
        fprintf(stderr, "cannot open %s \n", inputdir.c_str());
        exit(1);
    } else {
        while ((direntp = readdir(dir_ptr)) != NULL) {
            if (*direntp->d_name != '.') { //gia kathe country
                aggregatorStructures.countries->insertNode(new Country(direntp->d_name, inputdir + direntp->d_name));
            }
        }

        closedir(dir_ptr);
    }

    printf("--------------- Aggregator -----------------\n");
    aggregatorStructures.countries->printListDetails();
    printf("-------------------------- -----------------\n");
}

CountriesLinkedList * getCountriesAgg() {
    return aggregatorStructures.countries;
}

void assignCountries() {//anathetei xwres stous workers
    CountriesNode* current = aggregatorStructures.countries->head;
    int i = 0;
    while (current != NULL) {
        current->rec->worker_pid = aggregatorStructures.pids[i];
        current = current->next;

        i = i + 1;
        if (i >= aggregatorStructures.w) {
            i = 0;
        }
    }
}

void sendServerData(string ip, int port) { //apostoli sto server
    char * packet = new char[aggregatorStructures.bufferSize]();
    strcpy(packet, ip.c_str());

    for (int i = 0; i < aggregatorStructures.w; i++) {
        
        write(aggregatorStructures.s_fds[i], packet, aggregatorStructures.bufferSize);

    }

    sprintf(packet, "%d", port);

    for (int i = 0; i < aggregatorStructures.w; i++) {
        write(aggregatorStructures.s_fds[i], packet, aggregatorStructures.bufferSize);
    }


    delete [] packet;

}

void sendCountries() { //stelnei tis xwres stous workers
    char * packet = new char[aggregatorStructures.bufferSize]();

    for (int i = 0; i < aggregatorStructures.w; i++) {
        CountriesNode* current = aggregatorStructures.countries->head;
        while (current != NULL) {
            if (current->rec->worker_pid == aggregatorStructures.pids[i]) {
                strcpy(packet, current->rec->name.c_str());
                write(aggregatorStructures.s_fds[i], packet, aggregatorStructures.bufferSize);
            }
            current = current->next;
        }
        strcpy(packet, "-");
        write(aggregatorStructures.s_fds[i], packet, aggregatorStructures.bufferSize);
    }
    delete [] packet;
}

int * findPipeForCountry(string country) { //vriskei poio pipe antistoixei sto worker pou exei ayti tin country
    CountriesNode* current = aggregatorStructures.countries->head;
    while (current != NULL) {
        if (current->rec->name == country) {
            pid_t pid = current->rec->worker_pid;


            for (int i = 0; i < aggregatorStructures.w; i++) {
                if (aggregatorStructures.pids[i] == pid) {

                    int * fds = new int[2];
                    fds[0] = aggregatorStructures.s_fds[i];
                    fds[1] = aggregatorStructures.r_fds[i];
                    return fds;
                }
            }
        }
        current = current->next;
    }
    return NULL;
}

void listCountriesForAggregator() {
    aggregatorStructures.countries->printListDetails();
}

void initializePipes(int w) { //arxikopoiisi twn pipes
    aggregatorStructures.w = w;
    aggregatorStructures.s_names = new string[w]; //onomata twn sending pipes
    aggregatorStructures.r_names = new string[w]; //onomata twn receiving pipes

    for (int i = 0; i < w; i++) {
        stringstream ss;
        ss << i;
        string str = ss.str();

        aggregatorStructures.s_names[i] = "s" + str + ".pipe"; //send
        aggregatorStructures.r_names[i] = "r" + str + ".pipe"; //receive
    }
    //dimiourgw ta pipes
    for (int i = 0; i < w; i++) {
        mkfifo(aggregatorStructures.s_names[i].c_str(), 0644);
    }

    for (int i = 0; i < w; i++) {
        mkfifo(aggregatorStructures.r_names[i].c_str(), 0644);
    }

    aggregatorStructures.s_fds = new int[w];
    aggregatorStructures.r_fds = new int[w];

    for (int i = 0; i < w; i++) {
        aggregatorStructures.s_fds[i] = 0;
        aggregatorStructures.r_fds[i] = 0;
    }
}

void cleanupPipes() { //gia tin katastrofi tous

    for (int i = 0; i < aggregatorStructures.w; i++) {
        unlink(aggregatorStructures.s_names[i].c_str()); //remove a link to a file deletes a name from the file system
    }

    for (int i = 0; i < aggregatorStructures.w; i++) {
        unlink(aggregatorStructures.r_names[i].c_str());
    }

    delete [] aggregatorStructures.s_fds;
    delete [] aggregatorStructures.r_fds;

    delete [] aggregatorStructures.s_names;
    delete [] aggregatorStructures.r_names;
}

void initializeWorkers(int bufferSize, string serverIP, int serverPort) { //arxikopoiisi workers
    aggregatorStructures.pids = new pid_t[aggregatorStructures.w];
    aggregatorStructures.bufferSize = bufferSize;

    for (int i = 0; i < aggregatorStructures.w; i++) {
        pid_t pid = fork();

        if (pid > 0) {
            aggregatorStructures.pids[i] = pid;
            aggregatorStructures.s_fds[i] = open(aggregatorStructures.s_names[i].c_str(), O_WRONLY); // open pipes
            aggregatorStructures.r_fds[i] = open(aggregatorStructures.r_names[i].c_str(), O_RDONLY);

            if (aggregatorStructures.s_fds[i] < 0) {
                perror("pipe");
                exit(1);
            }

            if (aggregatorStructures.r_fds[i] < 0) {
                perror("pipe");
                exit(1);
            }



        } else if (pid == 0) {
            cout << "worker started " << endl;

            ChildParameters parameters; //apothikeuw tis parametrous gia ta pipes stis domes
            parameters.inputdir = aggregatorStructures.inputdir;
            parameters.bufferSize = aggregatorStructures.bufferSize;
            parameters.s_name = aggregatorStructures.s_names[i];
            parameters.r_name = aggregatorStructures.r_names[i];

            exit(main_worker(parameters));
        } else {
            cout << " error" << endl;
            exit(1);
        }
    }


}

void cleanupWorkers() { //stelnei sigkill stous workers


    // for (int i = 0; i < aggregatorStructures.w; i++) {
    // collectStatisticsWithSelect();}


    for (int i = 0; i < aggregatorStructures.w; i++) {
        kill(aggregatorStructures.pids[i], SIGKILL); 
    }

    for (int i = 0; i < aggregatorStructures.w; i++) {
        waitpid(aggregatorStructures.pids[i], NULL, 0);
    }

    delete [] aggregatorStructures.pids;
}

void writeLog() {//ftiaxnw to logfile
    string s = string("log_file_") + to_string(getpid()) + ".txt";

    ofstream fp(s.c_str()); //to ofstream grafei sto disko

    aggregatorStructures.countries->saveToDisk(fp);

    fp << "TOTAL " << aggregatorStructures.total << endl;
    fp << "SUCCESS " << aggregatorStructures.total - aggregatorStructures.errors << endl;
    fp << "FAIL " << aggregatorStructures.errors << endl;
}

void broadcast(string msg) { //to xrisimopoiw stis periptwseis tis select, wste na paroun oi workers tin entoli
    char * packet = new char[aggregatorStructures.bufferSize]();

    strcpy(packet, msg.c_str());

    for (int i = 0; i < aggregatorStructures.w; i++) {
        write(aggregatorStructures.s_fds[i], packet, aggregatorStructures.bufferSize);
    }
    delete [] packet;
}

void collectDiseaseFrequencyOneByOne() { //an den xrisimopoiousa select, kai ta epairna ena ena
    char * packet = new char[aggregatorStructures.bufferSize]();
    int sum = 0;

    for (int i = 0; i < aggregatorStructures.w; i++) {
        if (read(aggregatorStructures.r_fds[i], packet, aggregatorStructures.bufferSize) <= 0) { //pairnei tin apantisi
            break;
        }
        sum = sum + atoi(packet);
    }

    cout << sum << endl;

    delete [] packet;
}

void collectNumPatientWithSelect() {//i select gia ti numpatientAdmissions kai discharges
    char * packet = new char[aggregatorStructures.bufferSize]();
    fd_set active_fd_set;

    bool * answers_received = new bool[aggregatorStructures.w];

    for (int i = 0; i < aggregatorStructures.w; i++) {
        answers_received[i] = false; //arxikopoiw ta answers received
    }

    int answers_missing = aggregatorStructures.w;

    //    int sum = 0;

    while (answers_missing > 0) {
        FD_ZERO(&active_fd_set);

        for (int i = 0; i < aggregatorStructures.w; i++) {
            if (!answers_received[i]) {
                int fd = aggregatorStructures.r_fds[i];
                FD_SET(fd, &active_fd_set);
            }
        }

        if (select(FD_SETSIZE, &active_fd_set, NULL, NULL, NULL) < 0) {
            cout << "Select error !!!  " << endl;
            return;
        }

        for (int i = 0; i < aggregatorStructures.w; i++) {
            int fd = aggregatorStructures.r_fds[i];
            //xrisimopoiw maska gia na elegxw poies apantiseis exw lavei
            if (FD_ISSET(fd, &active_fd_set)) {
                if (read(aggregatorStructures.r_fds[i], packet, aggregatorStructures.bufferSize) <= 0) { //pairnei tin apantisi
                    break;
                }

                if (strcmp(packet, "###") != 0)
                    cout << packet;
                answers_received[i] = true;
                answers_missing--;
            }
        }
    }

    printf("Select finished ! \n");

    delete [] packet;

    delete [] answers_received;
}

void collectDiseaseFrequencyWithSelect() { //omoiws
    char * packet = new char[aggregatorStructures.bufferSize]();
    fd_set active_fd_set;

    bool * answers_received = new bool[aggregatorStructures.w];

    for (int i = 0; i < aggregatorStructures.w; i++) {
        answers_received[i] = false;
    }

    int answers_missing = aggregatorStructures.w;

    int sum = 0;

    while (answers_missing > 0) {
        FD_ZERO(&active_fd_set);

        for (int i = 0; i < aggregatorStructures.w; i++) {
            if (!answers_received[i]) {
                int fd = aggregatorStructures.r_fds[i];
                FD_SET(fd, &active_fd_set);
            }
        }

        if (select(FD_SETSIZE, &active_fd_set, NULL, NULL, NULL) < 0) {
            cout << "Select error !!!  " << endl;
            return;
        }

        for (int i = 0; i < aggregatorStructures.w; i++) {
            int fd = aggregatorStructures.r_fds[i];

            if (FD_ISSET(fd, &active_fd_set)) {
                if (read(aggregatorStructures.r_fds[i], packet, aggregatorStructures.bufferSize) <= 0) { //pairnei tin apantisi
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
}

void collectSearchRecordIdWithSelect() {//omoiws
    char * packet = new char[aggregatorStructures.bufferSize]();
    fd_set active_fd_set;

    bool * answers_received = new bool[aggregatorStructures.w];

    for (int i = 0; i < aggregatorStructures.w; i++) {
        answers_received[i] = false;
    }

    int answers_missing = aggregatorStructures.w;

    bool found = false;

    while (answers_missing > 0 && found == false) {
        FD_ZERO(&active_fd_set);

        for (int i = 0; i < aggregatorStructures.w; i++) {
            if (!answers_received[i]) {
                int fd = aggregatorStructures.r_fds[i];
                FD_SET(fd, &active_fd_set);
            }
        }

        if (select(FD_SETSIZE, &active_fd_set, NULL, NULL, NULL) < 0) {
            cout << "Select error !!!  " << endl;
            return;
        }

        for (int i = 0; i < aggregatorStructures.w; i++) {
            int fd = aggregatorStructures.r_fds[i];

            if (FD_ISSET(fd, &active_fd_set)) {
                if ((read(aggregatorStructures.r_fds[i], packet, aggregatorStructures.bufferSize) <= 0)) { //pairnei tin apantisi
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



    delete [] packet;

    delete [] answers_received;
}

void collectStatisticsWithSelect() { //i select gia ta statistics
    char * packet = new char[aggregatorStructures.bufferSize]();
    fd_set active_fd_set;

    bool * answers_received = new bool[aggregatorStructures.w];

    for (int i = 0; i < aggregatorStructures.w; i++) {
        answers_received[i] = false;
    }

    int answers_missing = aggregatorStructures.w;

    while (answers_missing > 0) {
        FD_ZERO(&active_fd_set);

        for (int i = 0; i < aggregatorStructures.w; i++) {
            if (!answers_received[i]) {
                int fd = aggregatorStructures.r_fds[i];
                FD_SET(fd, &active_fd_set);
            }
        }

        if (select(FD_SETSIZE, &active_fd_set, NULL, NULL, NULL) < 0) {
            cout << "Select error !!!  " << endl;
            return;
        }

        for (int i = 0; i < aggregatorStructures.w; i++) {
            int fd = aggregatorStructures.r_fds[i];

            if (FD_ISSET(fd, &active_fd_set)) {
                if (read(aggregatorStructures.r_fds[i], packet, aggregatorStructures.bufferSize) <= 0) { //pairnei tin apantisi
                    break;
                }


                cout << packet << endl;
                answers_received[i] = true;
                answers_missing--;
            }
        }
    }

    printf("Select finished ! \n");

    delete [] packet;

    delete [] answers_received;
}
