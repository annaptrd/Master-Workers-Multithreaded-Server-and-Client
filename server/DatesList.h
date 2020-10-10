#ifndef DATESLINKEDLISTTYPES_H
#define DATESLINKEDLISTTYPES_H

#include <string>

using namespace std;

class DatesListNode {
public:
    string date;
    DatesListNode* next;
};

class DatesList {
public:
    DatesList();
    void insertNode(string);
    bool insertNodeWithCheck(string);
    void deleteNode(string); 
    DatesListNode* searchList(string);
    DatesListNode* lastNode();
    void printList();
    ~DatesList();
    void destroyRecords();
    
    DatesListNode* head;
    int length = 0; 
    
    void sort();
};


#endif //MYCLASSES_Η