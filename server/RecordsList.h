#ifndef LINKEDLISTTYPES_H
#define LINKEDLISTTYPES_H
#include <string>

using namespace std;

#include "Record.h"


using namespace std;

class Node {
public:
    Record * rec;
    Node* next;
};

class LinkedList {
public:
    LinkedList();
    void insertNode(Record *);
    bool insertNodeWithCheck(Record *);
    void deleteNode(string); 
    Node* searchList(string);
    void printList();
    ~LinkedList();
    void destroyRecords();
    void saveToDisk2(ofstream & fp);
    int searchForDisease(string);
    int searchForCountryWithAge(string,int,int);
    int searchForCountry(string);
    Node* head;
    int length = 0; 
};


#endif //MYCLASSES_Η