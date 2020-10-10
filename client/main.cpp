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

#include "CommandsList.h"
#include "methods_client.h"

using namespace std;

void catchinterrupt_aggregator(int signo) { //ctr+C
    printf("\nCatching SIGINT: signo=%d\n", signo);

    cleanupWorkers();

    cout << "Bye." << endl;
    exit(0);
}

int main(int argc, char * argv[]) {
    int w, servPort;
    string queryFile;
    string servIP;
    int bufferSize = 5000;

    if (argc != 9) {
        printf("lathos parametroi \n");
        printf("prepei na einai: -q queryFile -w numThreads -sp servPort -sip servIP \n");
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
        printf("prepei na einai: -q queryFile -w numThreads -sp servPort -sip servIP \n");
        return 0;
    }
    if (string(argv[1]) == "-sp") {
        servPort = atoi(argv[2]);
    } else if (string(argv[3]) == "-sp") {
        servPort = atoi(argv[4]);
    } else if (string(argv[5]) == "-sp") {
        servPort = atoi(argv[6]);
    } else if (string(argv[7]) == "-sp") {
        servPort = atoi(argv[8]);
    } else {
        printf("lathos parametroi \n");
        printf("prepei na einai: -q queryFile -w numThreads -sp servPort -sip servIP \n");
        return 0;
    }
    if (string(argv[1]) == "-q") {
        queryFile = (argv[2]);
    } else if (string(argv[3]) == "-q") {
        queryFile = (argv[4]);
    } else if (string(argv[5]) == "-q") {
        queryFile = (argv[6]);
    } else if (string(argv[7]) == "-q") {
        queryFile = (argv[8]);
    } else {
        printf("lathos parametroi \n");
        printf("prepei na einai: -q queryFile -w numThreads -sp servPort -sip servIP \n");
        return 0;
    }

    if (string(argv[1]) == "-sip") {
        servIP = (argv[2]);
    } else if (string(argv[3]) == "-sip") {
        servIP = (argv[4]);
    } else if (string(argv[5]) == "-sip") {
        servIP = (argv[6]);
    } else if (string(argv[7]) == "-sip") {
        servIP = (argv[8]);
    } else {
        printf("lathos parametroi \n");
        printf("prepei na einai: -q queryFile -w numThreads -sp servPort -sip servIP \n");
        return 0;
    }

    printf("numThreads = %d \n", w);
    printf("servIP = %s \n", servIP.c_str());
    printf("servPort = %d \n", servPort);
    printf("queryFile = %s \n", queryFile.c_str());

    
    initializeCommands(queryFile);

    initializeW(w, servIP, servPort); //arxikopoiisi
        
    assignCommands();

    initializeWorkers(bufferSize);

    static struct sigaction act; 
    act.sa_handler = catchinterrupt_aggregator; 
    sigfillset(&(act.sa_mask)); 
    sigaction(SIGINT, &act, NULL); 
    sigaction(SIGQUIT, &act, NULL);
    
    //sigint Interrupt from keyboard
    //sigquit  Quit from keyboard


    printf("Waiting for threads to exit ... \n");
    
    cleanupWorkers();
    
    printf("Main thread finished ... \n");
    
    return 0;
}
