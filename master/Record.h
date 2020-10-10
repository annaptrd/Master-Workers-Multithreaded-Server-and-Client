#ifndef RECORD_H
#define RECORD_H

#include <string>

#define NO_DATE 0

using namespace std;

class Record {
public:
    Record(string[7]);
    virtual ~Record();
    
    string id;
    string firstname;
    string lastname;
    string disease;
    string country;
    int entrydate;
    int exitdate;     
    int age;
    
    void setExitDate(string exitdate);    
};

#endif /* RECORD_H */