
#ifndef CHILDPARAMETERS_H
#define CHILDPARAMETERS_H

#include <string>

#include "CommandsList.h"

using namespace std;

typedef struct ChildParameters {
    int i;
    int bufferSize;

    CommandsLinkedList * commandsList;
    string ip;
    int port;
    
    int * w_ready_pointer;
    int w;    
    
    pthread_mutex_t * m_pointer;
    pthread_cond_t * c_pointer;
} ChildParameters;

#endif /* CHILDPARAMETERS_H */

