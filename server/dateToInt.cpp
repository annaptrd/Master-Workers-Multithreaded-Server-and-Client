#include <iostream>
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include "Record.h"
#include <sstream>
#include <unistd.h>
#include <dirent.h>
#include <fstream>
#include <fcntl.h>
#include <cstring>


using namespace std;

int dateToInt(string d) {//metatrepw to date se int ("06-02-2016"->20160206) 
    int day, month, year;

    if (d == "" || d == "-") {
        return NO_DATE;
    }

    day = (d[0] - 48)*10 + d[1] - 48;
    month = (d[3] - 48)*10 + d[4] - 48;
    year = (d[6] - 48)*1000 + (d[7] - 48)*100 + (d[8] - 48)*10 + (d[9] - 48);

    int temp;
    temp = year * 10000 + month * 100 + day;
    return temp;
}

string intToDate(int n){//kanei to date apo int se string
    int temp,year,month,day;
    day=n%100;
    temp=n/100;
    month=temp%100;
    temp=temp/100;
    year=temp;
    string fullDate;
//    int a = 10;
    if(day<10 && month<10)
        fullDate = "0"+to_string(day) + "-"+ "0"+to_string(month) + "-" + to_string(year);
    else if(day<10)
        fullDate = "0"+to_string(day) + "-"+ to_string(month) + "-" + to_string(year);
    else if(month<10)
        fullDate = to_string(day) + "-"+ "0"+to_string(month) + "-" + to_string(year);
    else
        fullDate = to_string(day) + "-"+ to_string(month) + "-" + to_string(year);
    
    return fullDate;
}