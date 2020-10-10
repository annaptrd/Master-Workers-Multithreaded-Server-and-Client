#ifndef METHODS_AGGREGATOR_H
#define METHODS_AGGREGATOR_H

#include "Record.h"
#include "Data.h"
#include "CountriesList.h"
#include "queue.h"
#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <sys/types.h>	     /* sockets */
#include <sys/socket.h>	     /* sockets */
#include <netinet/in.h>	     /* internet sockets */
#include <netdb.h>	         /* gethostbyaddr */
using namespace std;

class ServerStructures {
public:
    int threads; //plithos workers
    int w; //plithos workers
    pthread_t * tids;
    int total; //gia to logfile
    int errors;
    CountriesLinkedList * countries;
    Queue * queue;
    int bufferSize;
    
    WorkersData* workers;
    
    // sockets
    struct sockaddr_in server_for_clients;
    struct sockaddr_in server_for_workers;
    
    pthread_mutex_t m;
     
    int socket_for_clients;
    int socket_for_workers;
    
};

ServerStructures * getServerStructures();


class PacketNode{
public:
    char* packet;
    PacketNode* next;
};

class PacketsList{
public:
    PacketsList();
    ~PacketsList();
    void insertNode(char* p);
    int length;
    PacketNode* head;
};



void assignCountries();

void sendCountries();

void aggrTotal();

void aggrErrors();

void initializeSockets(int, int);

void initializeWorkers(int);

void listCountriesForAggregator();

CountriesLinkedList* getCountriesAgg();


void cleanupSockets();

void cleanupWorkers();

int * broadcast(WorkersData* sworkers, string msg);

int * broadcastToOne(int port,string ip, string msg);

int connectTo(string ip, int port);

int collectDiseaseFrequencyWithSelect(int * fds, int size);

int read_all(int fd, void *buff, int size);

int write_all(int fd, const void *buff, int size) ;

char* collectPatientRecordWithSelect(int * fds, int size); 

int * waitForConnection();

#endif /* METHODS_H */
