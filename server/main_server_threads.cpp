#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include "dateToInt.h"
#include "BucketNode.h"
#include "methods.h"
#include "inbetweenDates.h"
#include "BST.h"
#include "CountriesList.h"
#include "DatesList.h"
#include "ChildParameters.h"
#include "methods_server.h"

using namespace std;

void * main_worker(void * args) {
    const ChildParameters * parameters = (ChildParameters *) args;
    const int bufferSize = parameters->bufferSize;
    ServerStructures * S = getServerStructures();

    cout << "Thread initialized: " << pthread_self() << " " << parameters->i << " " << endl;

    cout << "Thread started: " << pthread_self() << " " << parameters->i << " " << endl;

    while (true) {
        QueueItem * item = parameters->queue->obtain();
        const int * conndata = item->conndata;

        if (conndata == NULL) {
            break;
        }

        // service:
        if (conndata[1] == 0) { // client 
            char * packet = new char[bufferSize];

            int fd = conndata[0];
            read_all(fd, packet, bufferSize);

            pthread_mutex_lock(parameters->m_pointer);
            Client* newClient = new Client(packet);
            pthread_mutex_unlock(parameters->m_pointer);

            string line = newClient->query;
            stringstream command(line);
            string entoli;

            string data[8] = {"", "", "", "", "", "", "", ""};

            command >> data[0] >> data[1] >> data[2] >> data[3] >> data[4] >> data[5] >> data[6] >> data[7];

            if (data[0] == "/diseaseFrequency" || data[0] == "diseaseFrequency") {
                if (data[1] == "" || data[3] == "" || data[2] == "") {
                    cout << "error: you have to enter one virus name,two dates (and maybe a country-optional) " << endl;
                } else {
                    if (data[4] == "") { // without country
                        // select

                        cout << "Broadcasting to all: " << line << endl;
                        int * fds = broadcast(S->workers, line); //stelnei tin command stous workers

                        int sum = collectDiseaseFrequencyWithSelect(fds, S->workers->getSize());//pairnei tin apantisi apo tous workers

                        sprintf(packet, "%d", sum);
                        if (write_all(fd, packet, bufferSize) <= 0) { //stelnei tin apantisi ston client
                            cout << "Client could not get its answer \n";
                        }

                        pthread_mutex_lock(parameters->m_pointer);
                        cout << "******************************************\n";
                        cout << "*  cmd: " << line << endl;
                        cout << "*      " << sum << endl;
                        cout << "******************************************\n";
                        pthread_mutex_unlock(parameters->m_pointer);
                    } else { // with country

                        WorkerNode* current;
                        current = S->workers->searchForCountry(data[4]); //vriskei ton worker pou einai ypeuthynos gia ayti tin xwra

                        if (current != NULL) {
                            cout << "Broadcasting to one: " << line << endl;
                            int * fds = broadcastToOne(current->worker->port, current->worker->ip, line); //stelnei tn entoli s ayton ton sygkekrimeno worker

                            while (1) {
                                if (read_all(fds[0], packet, bufferSize) <= 0) { //pairnei tin apantisi                                    
                                    break;
                                }
                                if (strcmp(packet, "-") == 0) {
                                    break;
                                } else {
                                    int sum = atoi(packet);

                                    sprintf(packet, "%d", sum);
                                    if (write_all(fd, packet, bufferSize) <= 0) { //stelnei tin apantisi ston client
                                        cout << "Client could not get it's answer \n";
                                    }


                                    pthread_mutex_lock(parameters->m_pointer);
                                    cout << "******************************************\n";
                                    cout << "*  cmd: " << line << endl;
                                    cout << "*      " << packet << endl;
                                    cout << "******************************************\n";
                                    pthread_mutex_unlock(parameters->m_pointer);
                                    break;
                                }
                            }
                        } else {
                            int sum = 0;

                            sprintf(packet, "%d", sum);
                            if (write_all(fd, packet, bufferSize) <= 0) { //stelnei tin apantisi 0 an den ypirxe katholou
                                cout << "Client could not get it's answer \n";
                            }

                            pthread_mutex_lock(parameters->m_pointer);
                            cout << "******************************************\n";
                            cout << "*  cmd: " << line << endl;
                            cout << "*      " << sum << endl;
                            cout << "******************************************\n";
                            pthread_mutex_unlock(parameters->m_pointer);
                        }
                    }
                }
            } else if (data[0] == "/topk-AgeRanges" || data[0] == "topk-AgeRanges") {
                if (data[1] == "" || data[2] == "" || (data[3] == "" && data[4] != "") || (data[3] != "" && data[4] == "")) {
                    cout << "error: you have to enter a number(top k), a country ,a disease and two dates" << endl;
                } else {
                    WorkerNode* current;
                    current = S->workers->searchForCountry(data[2]); //vriskei ton armodio worker gia ti xwra
                    if (current != NULL) {
                        cout << "Broadcasting to one: " << line << endl;
                        int * fds = broadcastToOne(current->worker->port, current->worker->ip, line);//stelnei tin entoli sto worker
                        pthread_mutex_lock(parameters->m_pointer);
                        cout << "******************************************\n";
                        cout << "*  cmd: " << line << endl;
                        while (1) {
                        if (read_all(*fds, packet, bufferSize) <= 0) { //pairnei tin apantisi
                            break;
                        }
                        if (strcmp(packet, "-") == 0) {
                            break;
                        } else {
                            if (write_all(fd, packet, bufferSize) <= 0) { //stelnei tin apantisi ston client
                                cout << "Client could not get its answer \n";
                            }
                            cout << packet << endl;
                        }

                        }
                        cout << "******************************************\n";
                        pthread_mutex_unlock(parameters->m_pointer);

                    }
                }
               
            } else if (data[0] == "/numPatientAdmissions" || data[0] == "numPatientAdmissions") {
                if (data[1] == "" || data[3] == "" || data[2] == "") {
                    cout << "error: you have to enter one virus name,(maybe a country-optional) and two dates" << endl;
                } else {
                    if (data[4] == "") {
                      
                        int * fds = broadcast(S->workers, line); //stelnei se olous tous workers tin entoli

                       pthread_mutex_lock(parameters->m_pointer);
                        cout << "******************************************\n";
                        cout << "*  cmd: " << line << endl;
                        int size=S->workers->getSize();
                         char * packet = new char[bufferSize]();
                        
                         fd_set active_fd_set;
                            bool * answers_received = new bool[size];
                            for (int i = 0; i < size; i++) {
                                answers_received[i] = false;
                            }
                            int answers_missing = size;
                            while (answers_missing > 0) {
                                FD_ZERO(&active_fd_set);

                                for (int i = 0; i < size; i++) {
                                    if (!answers_received[i]) {
                                        FD_SET(fds[i], &active_fd_set);
                                    }
                                }
                                if (select(FD_SETSIZE, &active_fd_set, NULL, NULL, NULL) < 0) {
                                    cout << "Select error !!!  " << endl;
                                    break;
                                }
                                for (int i = 0; i < size; i++) { //gia kathe worker
                                    if (FD_ISSET(fds[i], &active_fd_set)) {
                                        if (read_all(fds[i], packet, bufferSize) <= 0) { //pairnei tin apantisi
                                            break;
                                        } 
                                        cout<<packet<<endl;
                                       
                                        if (write_all(fd, packet, bufferSize) <= 0) { //stelnei tin apantisi ston client
                                            cout << "Client could not get its answer \n";
                                        }

                                        answers_received[i] = true;
                                        answers_missing--;
                                    }   
                              }
                            }
                            
                        printf("Select finished ! \n");

                        cout << "******************************************\n";
                        pthread_mutex_unlock(parameters->m_pointer);
                    }else{
                        WorkerNode* current;
                        current = S->workers->searchForCountry(data[4]); //vriksei ton worker tis xwras aytis
                        if (current != NULL) {
                            cout << "Broadcasting to one: " << line << endl;
                            int * fds = broadcastToOne(current->worker->port, current->worker->ip, line); //stelnei tin entoli ston armodio worker
                            pthread_mutex_lock(parameters->m_pointer);
                            cout << "******************************************\n";
                            cout << "*  cmd: " << line << endl;
                            while (1) {
                            if (read_all(*fds, packet, bufferSize) <= 0) { //pairnei tin apantisi
                                break;
                            }
                            if (strcmp(packet, "-") == 0) {
                                break;
                            } else {
                                if (write_all(fd, packet, bufferSize) <= 0) { //stelnei tin apantisi ston client
                                    cout << "Client could not get its answer \n";
                                }
                                cout << packet << endl;
                            }

                            }
                            cout << "******************************************\n";
                            pthread_mutex_unlock(parameters->m_pointer);

                        }
                        }
                   
                }
            } else if (data[0] == "/searchPatientRecord" || data[0] == "searchPatientRecord") {
                if (data[1] == "") {
                    cout << "error: you have to enter one patient id" << endl;
                } else {
                    cout << "Broadcasting to all: " << line << endl;
                    int * fds = broadcast(S->workers, line); //stelnei tin entoli se olous tous workers

                    char* packet = collectPatientRecordWithSelect(fds, S->workers->getSize()); //pairnei tin apantisi
                    
                    if ((strcmp(packet, "###") == 0)) {
                        strcpy(packet, "Id Not Found");
                        if (write_all(fd, packet, bufferSize) <= 0) { //stelnei tin apantisi ston client
                        cout << "Client could not get it's answer \n";
                        }
                    }else{
                    if (write_all(fd, packet, bufferSize) <= 0) { //stelnei tin apantisi ston client
                        cout << "Client could not get it's answer \n";
                    }}

                    pthread_mutex_lock(parameters->m_pointer);
                    cout << "******************************************\n";
                    cout << "*  cmd: " << line << endl;
                    cout << "*      " << packet << endl;
                    cout << "******************************************\n";
                    pthread_mutex_unlock(parameters->m_pointer);
                }
            } else if (data[0] == "/numPatientDischarges" || data[0] == "numPatientDischarges") {
                if (data[1] == "" || data[2] == "" || data[3] == "") {
                    cout << "error: you have to enter one virus name,(maybe a country-optional) and two dates" << endl;
                } else {
                    if (data[4] == "") {
                     int * fds = broadcast(S->workers, line); //stelnei se olous tous workers tin command

                       pthread_mutex_lock(parameters->m_pointer);
                        cout << "******************************************\n";
                        cout << "*  cmd: " << line << endl;
                        int size=S->workers->getSize();
                         char * packet = new char[bufferSize]();
                        
                         fd_set active_fd_set;
                            bool * answers_received = new bool[size];
                            for (int i = 0; i < size; i++) {
                                answers_received[i] = false;
                            }
                            int answers_missing = size;
                            while (answers_missing > 0) {
                                FD_ZERO(&active_fd_set);

                                for (int i = 0; i < size; i++) {
                                    if (!answers_received[i]) {
                                        FD_SET(fds[i], &active_fd_set);
                                    }
                                }
                                if (select(FD_SETSIZE, &active_fd_set, NULL, NULL, NULL) < 0) {
                                    cout << "Select error !!!  " << endl;
                                    break;
                                }
                                for (int i = 0; i < size; i++) { //gia kathe worker
                                    if (FD_ISSET(fds[i], &active_fd_set)) {
                                        if (read_all(fds[i], packet, bufferSize) <= 0) { //pairnei tin apantisi
                                            break;
                                        } 
                                        cout<<packet<<endl;
                                       
                                        if (write_all(fd, packet, bufferSize) <= 0) { //stelnei tin apantisi ston client
                                            cout << "Client could not get its answer \n";
                                        }

                                        answers_received[i] = true;
                                        answers_missing--;
                                    }   
                              }
                            }            
                        printf("Select finished ! \n");

                        cout << "******************************************\n";
                        pthread_mutex_unlock(parameters->m_pointer);
                    }else{
                        WorkerNode* current;
                        current = S->workers->searchForCountry(data[4]); //vriskei ton armodio worker
                        if (current != NULL) {
                            cout << "Broadcasting to one: " << line << endl;
                            int * fds = broadcastToOne(current->worker->port, current->worker->ip, line); //tou stelnei tin entoli
                            pthread_mutex_lock(parameters->m_pointer);
                            cout << "******************************************\n";
                            cout << "*  cmd: " << line << endl;
                            while (1) {
                            if (read_all(*fds, packet, bufferSize) <= 0) { //pairnei tin apantisi
                                break;
                            }
                            if (strcmp(packet, "-") == 0) {
                                break;
                            } else {
                                if (write_all(fd, packet, bufferSize) <= 0) { //stelnei tin apantisi sto client
                                    cout << "Client could not get its answer \n";
                                }
                                cout << packet << endl;
                            }

                            }
                            cout << "******************************************\n";
                            pthread_mutex_unlock(parameters->m_pointer);

                        }
                    }}
                }
                

            delete newClient;

            delete [] packet;

            close(fd);
        }

        if (conndata[1] == 1) { // master
            char * packet = new char[bufferSize];

            Worker* newWorker = new Worker();

            int fd = conndata[0];

            read_all(fd, packet, bufferSize);

            printf("IP: %s \n", packet);
            newWorker->ip = packet; //vazw to ip tou worker sti domi

            read_all(fd, packet, bufferSize);

            printf("PORT: %s \n", packet);
            newWorker->port = atoi(packet);//vazw to port tou worker sti domi

            S->workers->insertNode(newWorker); //topothetw ton neo worker

            newWorker->countries = new CountriesLinkedList();

            while (true) {
                read_all(fd, packet, bufferSize);

                if (packet[0] == '-') {
                    break;
                } else {
                    string name = packet;
                    newWorker->countries->insertNode(new Country(name));
                }
            }


            delete [] packet;

            close(fd);
        }

        delete item;
    }

    cout << "Thread finish: " << pthread_self() << " " << parameters->i << " " << endl;


    return 0;
}
