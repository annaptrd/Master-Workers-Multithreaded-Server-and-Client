#include <iostream>
#include "CountriesList.h"
#include <cstdlib>

using namespace std;

CountriesLinkedList::CountriesLinkedList() {
    //    cout << "list created" << endl;
    head = NULL;
}

CountriesLinkedList::~CountriesLinkedList() { //katastrefei ti lista
    //   cout << "list destructed" << endl;
    CountriesNode* current;
    CountriesNode* previous;
    previous = head;
    current = head;
    while (current != NULL) {
        previous = current;
        current = current->next;
        delete previous;
    }
}

void CountriesLinkedList::deleteNode(string key) //gia diagrafi enos sygkekrimenou country
{
    CountriesNode* previous;
    CountriesNode* current = head;
    while ((current->rec->name) != key && current != NULL) {
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

CountriesNode* CountriesLinkedList::searchList(string key) { //psaxnei gia sygkekrimeno country
    CountriesNode* current = head;
    while ((current->rec->name) != key && current != NULL) {
        current = current->next;
    }
    if (current == NULL) {
        cout << "The record you are looking for does not exist" << endl;
        return NULL;
    } else {
        ;
        return current;
    }
}

void CountriesLinkedList::insertNode(Country * rec) //eisagwgi neou komvou sto telos
{
    CountriesNode* newnode = new CountriesNode;
    newnode->next = NULL;
    newnode->rec = rec;

    if (head == NULL) {
        head = newnode;
    } else {
        CountriesNode* current = head;
        while ((current->next) != NULL) {
            current = (current->next);
        }
        current->next = newnode;
    }
}

bool CountriesLinkedList::insertNodeWithCheck(Country * record) { //eisagwgi me elegxo an yparxei idi
    CountriesNode* newnode = new CountriesNode;
    newnode->next = NULL;
    newnode->rec = record;

    if (head == NULL) {
        head = newnode;
        return true;
    } else {
        CountriesNode* current = head;

        if (current->rec->name == record->name) {
            return false;
        }

        while ((current->next) != NULL) {
            if (current->rec->name == record->name) {
                return false;
            }

            current = (current->next);
        }

        if (current->rec->name == record->name) {
            return false;
        }

        current->next = newnode;
        return true;
    }
}

void CountriesLinkedList::printList(bool printpid) { //ektypwsi listas
    CountriesNode* current = head;
    while (current != NULL) {
        cout << "name: " << (current->rec->name);
        if (printpid) {
            cout << " pid: " << current->rec->worker_pid << endl;
        } else {
            cout << endl;
        }
        current = current->next;
    }
    cout << (current->rec->name) << endl;
}

void CountriesLinkedList::printListDetails() { //ektypwsi listas
    CountriesNode* current = head;
    while (current != NULL) {
        cout << /* "name: " <<*/ (current->rec->name);
        cout << /* " pid: "*/" " << current->rec->worker_pid;
        //cout << " path: " << current->rec->filesystem_path;
        cout << endl;
        current = current->next;
    }
}

void CountriesLinkedList::saveToDisk(ofstream & fp) { //grafei sto disko 
    CountriesNode* current = head;
    while (current != NULL) {
        fp << /* "name: " <<*/ (current->rec->name);
        //fp << /* " pid: "*/" " << current->rec->worker_pid;
        fp << endl;
        current = current->next;
    }
}

int CountriesLinkedList::searchForCountry(string c) { //anazitisi gia sygkekrimeni country
    int count = 0;
    CountriesNode* current = head;

    while (current != NULL) {
        if (current->rec->name == c) {
            count++;
        }
        current = current->next;
    }

    return count;
}

void CountriesLinkedList::destroyCountries() { //katastrofi kapoiou record
    CountriesNode* current = head;
    while (current != NULL) {
        delete current->rec;
        current = current->next;
    }
}

