#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <string>
#include  <sys/types.h>
#include  <dirent.h>
#include  <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include "methods_client.h"
#include "CommandsList.h"
#include "ChildParameters.h"

using namespace std;

void * main_worker(void * args) {
    ChildParameters * parameters = (ChildParameters *) args;
    cout << "Thread initialized: " << pthread_self() << " " << parameters->i << " " << parameters->ip << " " << parameters->port << endl;

    pthread_mutex_lock(parameters->m_pointer);

    (*(parameters->w_ready_pointer))++;

    while ((*(parameters->w_ready_pointer)) < parameters->w) {
        pthread_cond_wait(parameters->c_pointer, parameters->m_pointer);
    }

    pthread_cond_broadcast(parameters->c_pointer);

    pthread_mutex_unlock(parameters->m_pointer);

    cout << "Thread started: " << pthread_self() << " " << parameters->i << " " << parameters->ip << " " << parameters->port << endl;


    // barrier: //gia parallilia
    CommandsNode* current = parameters->commandsList->head;
    while (current != NULL) {
        if (current->command->worker_id == parameters->i) {
            cout << " tid: " << pthread_self() << " :" << current->command->query << endl;

            string line = current->command->query;
            stringstream command(current->command->query);

            string entoli;

            string data[8] = {"", "", "", "", "", "", "", ""};

            command >> data[0] >> data[1] >> data[2] >> data[3] >> data[4] >> data[5] >> data[6] >> data[7];

            if (data[0] == "/diseaseFrequency" || data[0] == "diseaseFrequency") {
                if (data[1] == "" || data[3] == "" || data[2] == "") {
                    cout << "error: you have to enter one virus name,two dates (and maybe a country-optional) " << endl;
                } else {
                    if (data[4] == "") {
                        // no country
                        // select

                        cout << "Sending to server: " << line << endl;
                        int fd = connectTo(parameters->ip, parameters->port);

                        sendToServer(fd, line, parameters->bufferSize); //stelnei to command sto server

                        char * packet = receiveFromServer(fd, parameters->bufferSize); //lambanei tin apantisi

                        pthread_mutex_lock(parameters->m_pointer);
                        cout << "******************************************\n";
                        cout << "*  cmd: " << line << endl;
                        cout << "*      " << packet << endl;
                        cout << "******************************************\n";
                        pthread_mutex_unlock(parameters->m_pointer);

                        delete [] packet;

                        close(fd);

                    } else {
                        cout << "Sending to server: " << line << endl;
                        int fd = connectTo(parameters->ip, parameters->port);

                        sendToServer(fd, line, parameters->bufferSize); //stelnei command sto server
                        
                        char * packet = receiveFromServer(fd, parameters->bufferSize); //lamvanei apantisi

                        pthread_mutex_lock(parameters->m_pointer);
                        cout << "******************************************\n";
                        cout << "*  cmd: " << line << endl;
                        cout << "*      " << packet << endl;
                        cout << "******************************************\n";
                        pthread_mutex_unlock(parameters->m_pointer);

                        delete [] packet;

                        close(fd);
                    }
                }
            } else if (data[0] == "/topk-AgeRanges" || data[0] == "topk-AgeRanges") {
                if (data[1] == "" || data[2] == "" || (data[3] == "" && data[4] != "") || (data[3] != "" && data[4] == "")) {
                    cout << "error: you have to enter a number(top k), a country ,a disease and two dates" << endl;
                } else {
                    cout << "Sending to server: " << line << endl;
                    int fd = connectTo(parameters->ip, parameters->port);
                    char * packet = new char[parameters->bufferSize];
                    sendToServer(fd, line, parameters->bufferSize);

                    pthread_mutex_lock(parameters->m_pointer);
                    cout << "******************************************\n";
                    cout << "*  cmd: " << line << endl;
                    while (1) {
                        if (read_all(fd, packet, parameters->bufferSize) <= 0) { //pairnei tin apantisi
                            break;
                        }
                        if (strcmp(packet, "-") == 0) {
                            break;
                        } else {
                            cout << packet << endl;
                        }

                    }
                    
                    cout << "******************************************\n";
                    pthread_mutex_unlock(parameters->m_pointer);

                    delete [] packet;


                    close(fd);
                }
            } else if (data[0] == "/numPatientAdmissions" || data[0] == "numPatientAdmissions") {
                if (data[1] == "" || data[3] == "" || data[2] == "") {
                    cout << "error: you have to enter one virus name,(maybe a country-optional) and two dates" << endl;
                } else {
                    cout << "Sending to server: " << line << endl;
                    int fd = connectTo(parameters->ip, parameters->port);
                    char * packet = new char[parameters->bufferSize];
                    sendToServer(fd, line, parameters->bufferSize);

                    pthread_mutex_lock(parameters->m_pointer);
                    cout << "******************************************\n";
                    cout << "*  cmd: " << line << endl;
                    
                    while (1) {
                                if (read_all(fd, packet,parameters->bufferSize) <= 0) { //pairnei tin apantisi
                                    break;
                                }
                                if (strcmp(packet, "-") == 0) {cout << packet << endl;
                                    break;
                                } else {
                                    cout << packet << endl;
                                }

                            }           
                    cout << "******************************************\n";
                    pthread_mutex_unlock(parameters->m_pointer);
                    delete [] packet;
                    close(fd);
                }
            } else if (data[0] == "/searchPatientRecord" || data[0] == "searchPatientRecord") {
                if (data[1] == "") {
                    cout << "error: you have to enter one patient id" << endl;
                } else {
                    cout << "Sending to server: " << line << endl;
                    int fd = connectTo(parameters->ip, parameters->port);

                    sendToServer(fd, line, parameters->bufferSize);
                    
                    char * packet = new char[parameters->bufferSize]();

                    if (read_all(fd, packet, parameters->bufferSize) <= 0) { //pairnei tin apantisi
                        return NULL;
                    }
                    
                    pthread_mutex_lock(parameters->m_pointer);
                    cout << "******************************************\n";
                    cout << "*  cmd: " << line << endl;
                    cout << "*      " << packet << endl;
                    cout << "******************************************\n";
                    pthread_mutex_unlock(parameters->m_pointer);


                    delete [] packet;



                    close(fd);
                }
            } else if (data[0] == "/numPatientDischarges" || data[0] == "numPatientDischarges") {
                if (data[1] == "" || data[2] == "" || data[3] == "") {
                    cout << "error: you have to enter one virus name,(maybe a country-optional) and two dates" << endl;
                } else {
                    cout << "Sending to server: " << line << endl;
                    int fd = connectTo(parameters->ip, parameters->port);
                    char * packet = new char[parameters->bufferSize];
                    sendToServer(fd, line, parameters->bufferSize);

                    pthread_mutex_lock(parameters->m_pointer);
                    cout << "******************************************\n";
                    cout << "*  cmd: " << line << endl;
                    
                    while (1) {
                                if (read_all(fd, packet,parameters->bufferSize) <= 0) { //pairnei tin apantisi
                                    break;
                                }
                                if (strcmp(packet, "-") == 0) {cout << packet << endl;
                                    break;
                                } else {
                                    cout << packet << endl;
                                }

                            }           
                    cout << "******************************************\n";
                    pthread_mutex_unlock(parameters->m_pointer);
                    delete [] packet;
                    close(fd);
                }
                
               
            }
        }

        current = current->next;
    }



    cout << "Thread finished: " << pthread_self() << endl;

   

    return 0;
}
