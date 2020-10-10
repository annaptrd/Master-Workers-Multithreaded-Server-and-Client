#ifndef BUCKETNODE_H
#define BUCKETNODE_H

#include "Data.h"


class BucketNode {
public:
    BucketNode(int bucketsize);
    virtual ~BucketNode();
    
    Data * bucketptr;    
    BucketNode * next;
    int bucketsize;
private:

};

#endif /* BUCKETNODE_H */
