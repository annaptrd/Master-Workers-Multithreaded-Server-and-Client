#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

#include "dateToInt.h"

using namespace std;

bool inbetweenDates(string d1, string d2, string d3) {  //elegxei an einai anamesa stis imerominies
    int date1,date2,date3;
    date1=dateToInt(d1);
    date2=dateToInt(d2);
    date3=dateToInt(d3);
    
    if(date1<=date2 && date1>=date3)
        return  true;
    else if(date1>=date2 && date1<=date3)
        return true;
    else
        return false;
}

bool inbetweenDates(int d1, int d2, int d3) { //elegxei an einai anamesa stis imerominies
    return (d1 <= d2 && d1 >= d3) || (d1 <= d3 && d1 >= d2);
}
    
