#include <iostream>
#include "RecordsList.h"
#include <cstdlib>
#include <unistd.h>

using namespace std;


LinkedList::LinkedList() {
        cout << "list created by" << getpid() << endl;
    head = NULL;
}

LinkedList::~LinkedList() { //katastrefei ti lista
 //   cout << "list destructed" << endl;
    Node* current;
    Node* previous;
    previous = head;
    current = head;
    while (current != NULL) {
        previous = current;
        current = current->next;
        delete previous;
    }
}

void LinkedList::saveToDisk2(ofstream & fp) { //grafei sto disko
    Node* current = head;
    while (current != NULL) {
//        fp << (current->rec->country);
        //fp <<  " pid:  " << current->rec->worker_pid;
       // fp << endl;
        current = current->next;
    }
}

void LinkedList::deleteNode(string key) //gia diagrafi enos sygkekrimenou record
{
    Node* previous;
    Node* current = head;
    while ((current->rec->id) != key && current != NULL) {
        previous = current;
        current = current->next;
    }
    if (current == NULL)
        cout << "The record you are attempting to delete does not exist" << endl;
    else {
        previous->next = current->next;
        delete current;
    } 
}

Node* LinkedList::searchList(string key) { //psaxnei gia sygkekrimeno id 
    Node* current = head;
    
    while (current != NULL && (current->rec->id) != key) {
        current = current->next;
    }
    if (current == NULL) {
       // cout << "The record you are looking for does not exist" << endl;
        return NULL;
    } else { 
        return current;
    }
}

void LinkedList::insertNode(Record * rec) //eisagwgi neou komvou sto telos
{
    Node* newnode = new Node;
    newnode->next = NULL;
    newnode->rec = rec;

    if (head == NULL) {
        head = newnode;
    } else {
        Node* current = head;
        while ((current->next) != NULL) {
            current = (current->next);
        }
        current->next = newnode;
    }
}

bool LinkedList::insertNodeWithCheck(Record * record) { //eisagwgi me elegxo an yparxei idi
    Node* newnode = new Node;
    newnode->next = NULL;
    newnode->rec = record;

    if (head == NULL) {
        head = newnode;
        return true;
    } else {
        Node* current = head;

        if (current->rec->id == record->id) {
            return false;
        }

        while ((current->next) != NULL) {
            if (current->rec->id == record->id) {
                return false;
            }

            current = (current->next);
        }

        if (current->rec->id == record->id) {
            return false;
        }

        current->next = newnode;
        return true;
    }
}

void LinkedList::printList() { //ektypwsi listas
    Node* current = head;
    while (current != NULL) {
        cout << "id: " << (current->rec->id) << " first name: " << current->rec->firstname << " last name: " << current->rec->lastname << " disease: " << current->rec->disease << " country: " << current->rec->country << " entry: " << current->rec->entrydate << " exit: " << current->rec->exitdate << endl;
        current = current->next;
    }
    cout << (current->rec->id) << endl;
}

int LinkedList::searchForCountry(string c) { //anazitisi gia sygkekrimeni country
    int count = 0;
    Node* current = head;
    
    while (current != NULL) {
        if (current->rec->country == c)  {
            count++;
        }
        current = current->next;
    }

    return count;
}

int LinkedList::searchForDisease(string d) { //anazitisi gia sygkekrimeni d
    int count = 0;
    Node* current = head;
    
    while (current != NULL) {
        if (current->rec->disease == d)  {
            count++;
        }
        current = current->next;
    }

    return count;
}

int LinkedList::searchForCountryWithAge(string c,int a1,int a2) { //anazitisi gia sygkekrimeni country
    int count = 0;
    Node* current = head;
    
    while (current != NULL) {
        if (current->rec->country == c)  {
            if(current->rec->age <=a2 && current->rec->age>=a1){             
                count++;   
            }
        }
        current = current->next;
    }

    return count;
}
void LinkedList::destroyRecords() { //katastrofi kapoiou record
    Node* current = head;
    while (current != NULL) {
        delete current->rec;
        current = current->next;
    }
}

