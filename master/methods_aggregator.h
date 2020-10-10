#ifndef METHODS_AGGREGATOR_H
#define METHODS_AGGREGATOR_H

#include "Record.h"
#include "CountriesList.h"
#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <cstdlib>
using namespace std;

class AggregatorStructures {
public:
    string * s_names;//onomata pipes pou stelnoun
    string * r_names;//onomata pipes pou lamvanoun
    int * s_fds;
    int * r_fds;
    int w; //plithos workers
    pid_t * pids;
    int total; //gia to logfile
    int errors;
    CountriesLinkedList * countries;
    string inputdir;
    int bufferSize;
};

AggregatorStructures * getAggregatorStructures();

void initializeCountries(string inputdir); 

void assignCountries();

void sendServerData(string ip, int port);

void sendCountries();

void aggrTotal();

void aggrErrors();

void initializePipes(int);

void initializeWorkers(int, string, int);

void listCountriesForAggregator();

CountriesLinkedList* getCountriesAgg();

int * findPipeForCountry(string country);

void collectStatisticsWithSelect();

void cleanupPipes();

void cleanupWorkers();

void writeLog();

void broadcast(string msg);

void collectDiseaseFrequencyOneByOne();

void collectDiseaseFrequencyWithSelect();

void collectNumPatientWithSelect();

void collectSearchRecordIdWithSelect();

#endif /* METHODS_H */
