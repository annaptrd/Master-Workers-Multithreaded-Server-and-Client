#ifndef QUEUE_H
#define QUEUE_H

class QueueItem {
public:

    const int * conndata;

    QueueItem(int* conndata) : conndata(conndata) {
    }

};

class Queue {
private:
    const int capacity;
    QueueItem **data;

    pthread_mutex_t mtx;
    pthread_cond_t cond_nonempty;
    pthread_cond_t cond_nonfull;

    int start;
    int end;
    int count;

public:
    Queue(int capacity);
    ~Queue();


    void place(QueueItem * item);

    QueueItem * obtain();

};
#endif /* QUEUE_H */

