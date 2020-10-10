
#ifndef CHILDPARAMETERS_H
#define CHILDPARAMETERS_H

#include <string>
#include <signal.h>

#include "queue.h"

using namespace std;

typedef struct ChildParameters {
    int i;
    int bufferSize;

    pthread_mutex_t * m_pointer;
    Queue * queue;

} ChildParameters;

#endif /* CHILDPARAMETERS_H */

