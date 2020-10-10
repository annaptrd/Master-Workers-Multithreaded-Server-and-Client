#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <string>
#include  <sys/types.h>
#include  <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <cstring>

#include "Data.h"
#include "BucketNode.h"
#include "inbetweenDates.h"
#include "BST.h"
#include "CountriesList.h"
#include "methods_server.h"
#include "queue.h"

using namespace std;


void catchinterrupt_server(int signo) { //ctr+C
    printf("\nCatching SIGINT: signo=%d\n", signo);

    cleanupWorkers();
    cleanupSockets();

    cout << "Bye." << endl;
    exit(0);
}

int main(int argc, char * argv[]) {
    int w, queueBufferSize, queryPort, statsPort;
    string inputdir;


    if (argc != 9) {
        printf("lathos parametroi \n");
        printf("prepei na einai: -q queryPortNum -s statisticsPortNum -w numThreads -b bufferSize \n");
        return 0;
    }
    //elegxos gia orismata
    if (string(argv[1]) == "-w") {
        w = atoi(argv[2]);
    }//h thesh tou h1
    else if (string(argv[3]) == "-w") {
        w = atoi(argv[4]);
    } else if (string(argv[5]) == "-w") {
        w = atoi(argv[6]);
    } else if (string(argv[7]) == "-w") {
        w = atoi(argv[8]);
    } else {
        printf("lathos parametroi \n");
        printf("prepei na einai: -q queryPortNum -s statisticsPortNum -w numThreads -b bufferSize \n");
        return 0;
    }
    if (string(argv[1]) == "-b") {
        queueBufferSize = atoi(argv[2]);
    } else if (string(argv[3]) == "-b") {
        queueBufferSize = atoi(argv[4]);
    } else if (string(argv[5]) == "-b") {
        queueBufferSize = atoi(argv[6]);
    } else if (string(argv[7]) == "-b") {
        queueBufferSize = atoi(argv[8]);
    } else {
        printf("lathos parametroi \n");
        printf("prepei na einai: -q queryPortNum -s statisticsPortNum -w numThreads -b bufferSize \n");
        return 0;
    }
    if (string(argv[1]) == "-q") {
        queryPort = atoi(argv[2]);
    } else if (string(argv[3]) == "-q") {
        queryPort = atoi(argv[4]);
    } else if (string(argv[5]) == "-q") {
        queryPort = atoi(argv[6]);
    } else if (string(argv[7]) == "-q") {
        queryPort = atoi(argv[8]);
    } else {
        printf("lathos parametroi \n");
        printf("prepei na einai: -q queryPortNum -s statisticsPortNum -w numThreads -b bufferSize \n");
        return 0;
    }
    if (string(argv[1]) == "-s") {
        statsPort = atoi(argv[2]);
    } else if (string(argv[3]) == "-s") {
        statsPort = atoi(argv[4]);
    } else if (string(argv[5]) == "-s") {
        statsPort = atoi(argv[6]);
    } else if (string(argv[7]) == "-s") {
        statsPort = atoi(argv[8]);
    } else {
        printf("lathos parametroi \n");
        printf("prepei na einai: -q queryPortNum -s statisticsPortNum -w numThreads -b bufferSize \n");
        return 0;
    }
    //    bool messages = true;

    printf("buffersize = %d \n", queueBufferSize);
    printf("numThreads = %d \n", w);
    printf("queryPort   = %d \n", queryPort);
    printf("statsPort = %d \n", statsPort);

    ServerStructures * S = getServerStructures(); 
    S->bufferSize = 5000;
    S->threads = w;
    S->workers = new WorkersData();
    S->queue = new Queue(queueBufferSize);

    initializeSockets(queryPort, statsPort); //arxikopoiisi

    initializeWorkers(queueBufferSize);

    static struct sigaction act; 
    act.sa_handler = catchinterrupt_server; 
    sigfillset(&(act.sa_mask)); 
    sigaction(SIGINT, &act, NULL); 
    sigaction(SIGQUIT, &act, NULL);

    string line = "";

    int * conndata;

    cout << "Waiting for a connection ... \n";

    while ((conndata = waitForConnection()) != NULL) { //xrisimopoiw to conndata gia na kserw me poion syndethike k na pairnw tin pliroforia
        printf("Connection data on main thread: FD:%d | %d \n", conndata[0], conndata[1]);

        S->queue->place(new QueueItem(conndata));
    }
    
    delete S->queue;
    

    cleanupWorkers();
    
    cleanupSockets();

    cout << "Bye." << endl;

    return 0;
}
