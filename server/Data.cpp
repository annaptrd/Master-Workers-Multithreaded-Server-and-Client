#include "Data.h"
#include <iostream>
#include "CountriesList.h"
#include <cstdlib>
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

WorkerNode* WorkersData::searchForCountry(string c){
    pthread_mutex_lock(&workersMutex);
    WorkerNode* current=head;
    int check;
    while (current != NULL) {
    //    cout <<"current worker's port: "<< (current->worker->port);
    //    cout << endl;
        check=current->worker->countries->searchForCountry(c);
        if(check>0){
            pthread_mutex_unlock(&workersMutex);
            return current;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&workersMutex);
    return NULL;
}
void WorkersData::insertNode(Worker * w) //eisagwgi neou komvou sto telos
{
    pthread_mutex_lock(&workersMutex);
    WorkerNode* newnode = new WorkerNode;
    newnode->next = NULL;
    newnode->worker = w;

    if (head == NULL) {
        head = newnode;
    } else {
        WorkerNode* current = head;
        while ((current->next) != NULL) {
            current = (current->next);
        }
        current->next = newnode;
    }
    
    size++;
    pthread_mutex_unlock(&workersMutex);
}

 int WorkersData::getSize() {
     int copy;
     pthread_mutex_lock(&workersMutex);
     copy= size;
     pthread_mutex_unlock(&workersMutex);
     
     return copy;
 }

void WorkersData::printList() { //ektypwsi listas
    pthread_mutex_lock(&workersMutex);
    WorkerNode* current = head;
    while (current != NULL) {
        cout <<"current worker's port: "<< (current->worker->port);
        cout << endl;
        current = current->next;
    }
    pthread_mutex_unlock(&workersMutex);
}


WorkersData::WorkersData() : size(0) {
    pthread_mutex_init(&workersMutex, NULL);
    
    pthread_mutex_lock(&workersMutex);
    cout<<"WorkersData created"<<endl;
    head = NULL;
    pthread_mutex_unlock(&workersMutex);
}

WorkersData::~WorkersData(){
    WorkerNode* current;
    WorkerNode* previous;
    previous = head;
    current = head;
    while (current != NULL) {
        previous = current;
        current = current->next;
        delete previous;
    }
    
    pthread_mutex_destroy(&workersMutex);
}







void ClientData::insertNode(Client * c) //eisagwgi neou komvou sto telos
{
    ClientNode* newnode = new ClientNode;
    newnode->next = NULL;
    newnode->client = c;

    if (head == NULL) {
        head = newnode;
    } else {
        ClientNode* current = head;
        while ((current->next) != NULL) {
            current = (current->next);
        }
        current->next = newnode;
    }
}


void ClientData::printList() { //ektypwsi listas
    ClientNode* current = head;
    while (current != NULL) {
        cout <<"current client's query: "<< (current->client->query);
        cout << endl;
        current = current->next;
    }
}


ClientData::ClientData(){
    cout<<"ClientData created"<<endl;
    head = NULL;
}

ClientData::~ClientData(){
    ClientNode* current;
    ClientNode* previous;
    previous = head;
    current = head;
    while (current != NULL) {
        previous = current;
        current = current->next;
        delete previous;
    }
}
///////////////////////////////////
Data::Data() {
    empty = true; //an einai adeio
}

Data::~Data() {

}

