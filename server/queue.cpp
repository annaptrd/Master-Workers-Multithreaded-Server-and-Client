#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "queue.h"

Queue::Queue(int capacity) : capacity(capacity) { //oura me xwritikotita isi me capacity
    this->start = 0;
    this->end = -1;
    this->count = 0;

    pthread_mutex_init(&mtx, 0);
    pthread_cond_init(&cond_nonempty, 0);
    pthread_cond_init(&cond_nonfull, 0);

    data = new QueueItem*[capacity];

    for (int i = 0; i < capacity; i++) {
        data[i] = NULL;
    }
}

Queue::~Queue() { //katastrofi ouras
    pthread_cond_destroy(&cond_nonempty);
    pthread_cond_destroy(&cond_nonfull);
    pthread_mutex_destroy(&mtx);

    for (int i = 0; i < capacity; i++) {
        if (data[i] != NULL) {
            delete data[i];
        }
    }
    delete [] data;
}

void Queue::place(QueueItem * item) { //topothetisi stin oura
    pthread_mutex_lock(&mtx);

    while (this->count >= capacity) {
        pthread_cond_wait(&cond_nonfull, &mtx);
    }
    this->end = (this->end + 1) % capacity;
    this->data[this->end] = item;
    this->count++;

    pthread_cond_signal(&cond_nonempty);

    pthread_mutex_unlock(&mtx);
}

QueueItem * Queue::obtain() {//fifo 
    pthread_mutex_lock(&mtx);

    while (this->count <= 0) {
        pthread_cond_wait(&cond_nonempty, &mtx);
    }
    QueueItem * data = this->data[this->start];
    this->data[this->start] = NULL;
    this->start = (this->start + 1) % capacity;
    this->count--;
    pthread_mutex_unlock(&mtx);

    pthread_cond_signal(&cond_nonfull);

    return data;
}
