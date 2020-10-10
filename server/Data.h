#ifndef DATA_H
#define DATA_H

#include <iostream>
#include <string>
#include "CountriesList.h"
#include "BST.h"

using namespace std;

class Worker{
public:
    string ip;
    int port;
    //STATS
    CountriesLinkedList* countries;
};

class WorkerNode{
public:
    Worker* worker;
    WorkerNode* next;
};

class WorkersData{
public:
    WorkersData();
    ~WorkersData();
    void printList();
    void insertNode(Worker * );
    WorkerNode* searchForCountry(string c);
    WorkerNode* head;
    
    int getSize();
    
private:
    int size;
    pthread_mutex_t workersMutex;
};








class Client{
public:
    string query;
    //string ip;
    //int port;
    
    Client(string query) : query(query) {
        cout << "New client c-ted for query: " << query << endl;
    }

    ~Client() {
        cout << "Client d-ted \n";
    }
    
};

class ClientNode{
public:
    Client* client;
    ClientNode* next;
};

class ClientData{
public:
    ClientData();
    ~ClientData();
    void printList();
    void insertNode(Client * );
    ClientNode* head;
    
};



////////////////////////////////////////////////
class Data {
public:
    Data();
    virtual ~Data();
    
    string text; // name of the disease or name of the country
    BST bst; //to dentro me tous asthenis gia tin kathe disease/country
    bool empty;//an einai adeio
private:

};

#endif /* DATA_H */
