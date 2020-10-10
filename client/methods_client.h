#ifndef METHODS_CLIENT_H
#define METHODS_CLIENT_H

#include "CommandsList.h"
#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <cstdlib>

using namespace std;

class ClientStructures {
public:
    int w; //plithos workers
    pthread_t * tids;
    int total; //gia to logfile
    int errors;
    CommandsLinkedList * commandsList;
    string queryFile;
    int bufferSize;
    
    string ip;
    int port;
    
    pthread_mutex_t m;
    pthread_cond_t c;
};

ClientStructures * getAggregatorStructures();

void initializeCommands(string inputdir); 

void assignCommands();

void initializeWorkers(int);

CommandsLinkedList* getCommandsAgg();

void initializeW(int, string, int);

void cleanupWorkers();

int connectTo(string ip, int port);

void sendToServer(int fd, string msg, int bufferSize);

int write_all(int fd, const void *buff, int size);

int read_all(int fd, void *buff, int size);

char * receiveFromServer(int fd, int bufferSize);


#endif /* METHODS_H */
