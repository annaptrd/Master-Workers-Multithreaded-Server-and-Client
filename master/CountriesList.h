#ifndef COUNTRIES_LINKEDLIST_H
#define COUNTRIES_LINKEDLIST_H

#include <string>
#include <fstream>

#include "DatesList.h"

using namespace std;

class Country {
public:
    string name;
    pid_t worker_pid;
    string filesystem_path;
    DatesList dates;

    Country(string name, string filesystem_path = "", pid_t worker_pid = 0) : name(name), worker_pid(worker_pid), filesystem_path(filesystem_path) {

    }

};

class CountriesNode {
public:
    Country * rec;
    CountriesNode* next;
};

class CountriesLinkedList {
public:
    CountriesLinkedList();
    ~CountriesLinkedList();
    void insertNode(Country *);
    bool insertNodeWithCheck(Country *);
    void deleteNode(string); //record
    CountriesNode* searchList(string);
    void printList(bool printpid = false);
    void printListDetails();
    void saveToDisk(ofstream & fp);
    void destroyCountries();
    

    int searchForCountry(string c);
    CountriesNode* head;
    int length = 0;
};



#endif //MYCLASSES_Η