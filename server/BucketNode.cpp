#include "BucketNode.h"
#include <iostream>
using namespace std;


BucketNode::BucketNode(int bucketsize) : bucketsize(bucketsize) {
    bucketptr = new Data[bucketsize]; //deiktis se data
    next = NULL;

}

BucketNode::~BucketNode() {
   // cout<<"bucket destroyed"<<endl;
    delete [] bucketptr;
}
