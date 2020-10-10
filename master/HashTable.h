#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "Record.h"
#include "Data.h"
#include "BucketNode.h"

class HashTable {
    
public:
    HashTable();
    HashTable(const HashTable & cop);
    virtual ~HashTable();
    void initializeBuckets(int h, int bucketsize);
    void insertWithCountry(Record * record);
    void insertWithDisease(Record * record);
    Data * searchWithCountry(string country);
    Data * searchWithDisease(string disease);
    void printDiseasesStats();
    void printGlobalStatsWithDates(int,int);
    void printGlobalStats();
    void printHT();
    void saveToDisk(ofstream & fp);
    int bucketsize;
    int h;
    bool initialized;
    BucketNode** ht;
    
    void insert(string key, Record * record);
    Data * search(string key);
    
};

#endif /* HASHTABLE_H */
