#ifndef DATETOINT_H
#define DATETOINT_H

#include <string>

using namespace std;
int dateToInt(string);

string intToDate(int n);
bool inbetweenDates(string d1, string d2, string d3);
bool inbetweenDates(int d1, int d2, int d3);

#endif /* DATETOINT_H */