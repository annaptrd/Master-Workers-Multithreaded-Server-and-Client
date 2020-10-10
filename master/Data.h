#ifndef DATA_H
#define DATA_H

#include <string>

#include "BST.h"

using namespace std;

class Data {
public:
    Data();
    virtual ~Data();
    
    string text; // name of the disease or name of the country
    BST bst; //to dentro me tous asthenis gia tin kathe disease/country
    bool empty;//an einai adeio
private:

};

#endif /* DATA_H */
