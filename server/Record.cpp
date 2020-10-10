#include "Record.h"
#include "dateToInt.h"

Record::Record(string data[7]) {
    int i = 0;
    id = data[i++];
    firstname = data[i++];
    lastname = data[i++];
    disease = data[i++];
    country = data[i++];

    entrydate = dateToInt(data[i++]);
    exitdate = dateToInt(data[i++]);
    age = atoi(data[i].c_str());

}

Record::~Record() {

}

void Record::setExitDate(string exitdate) {    
    this->exitdate = dateToInt(exitdate);
}
