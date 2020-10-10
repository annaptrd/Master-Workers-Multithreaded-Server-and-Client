#include <string>
#include "hashFunction.h"

using namespace std;

#define MULTIPLIER 31 //tou vivliou alliws 37 


unsigned int hashFunction(string sstr) {
    unsigned char *p;

    const char * str = sstr.c_str();
    
    int h = 0;
    for (p = (unsigned char*) str; *p != '\0'; p++) {
        h = MULTIPLIER * h + *p;
    }

    return h; // or, h % ARRAY_SIZE;
}

/*The hash function returns the result modulo the size of the array. If  the hash function distributes key values uniformly, 
 * the  precise array size doesn't matter. It's hard to be certain  that a hash function is dependable, though, 
 * and even  the  best  function may have  trouble with some input sets, so it's wise to make the array size  a  prime 
 * number to give a  bit of extra  insurance by guaranteeing that the array size, the hash multiplier, and likely data values 
 * have no common factor. */
/*

The Practice of Programming (Addison-Wesley Professional Computing Series) 1st Edition

(HASH TABLES, pg. 57)

http://index-of.co.uk/Etc/The.Practice.of.Programming.-.B.W..Kernighan..pdf

 */