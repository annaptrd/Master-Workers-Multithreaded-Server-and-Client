#ifndef METHODS_H
#define METHODS_H

#include "Record.h"
#include "ChildParameters.h"
#include "OutputParameters.h"
#include "CountriesList.h"
#include <string>

using namespace std;

void initializeHashtables(int, int, int);

void insertPatientRecord(bool,Record *);

void recordPatientExit(bool,string, string);

void numCurrentPatients(bool,string *);

void writeLogWorker();

void workerErrors();

void workerTotal();

void globalDiseaseStats(bool,string*, string* );

void diseaseFrequency(bool,string ,string* ,string , string, OutputParameters &output );

void topk_Diseases(bool,int ,string ,string* ,string*);

void topk_Countries(bool,int ,string ,string* ,string*);

void topk_AgeRanges(bool,int ,string,string,string,string,OutputParameters &output );

void statistics(string,string,string,string,int*);

void numPatientAdmissions(bool,string,string,string,string*,OutputParameters & output);

void numPatientDischarges(bool,string,string,string,string*,OutputParameters & output);

void setChildParams(ChildParameters*);

void summaryStatistics(string , string,OutputParameters & );

ChildParameters* getChildParams();

void setCountriesL(CountriesLinkedList*);

CountriesLinkedList* getCountriesL();

void searchPatientRecord(bool,string,OutputParameters & output,bool*);

void cleanupStructures();

void * main_worker(void * args);

#endif /* METHODS_H */