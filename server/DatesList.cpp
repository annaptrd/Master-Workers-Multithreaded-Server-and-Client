#include <iostream>
#include <cstdlib>
#include "DatesList.h"
#include "dateToInt.h"
#include <cstring>
using namespace std;

DatesList::DatesList() {
    //    cout << "list created" << endl;
    head = NULL;
}

DatesList::~DatesList() { //katastrefei ti lista
    //   cout << "list destructed" << endl;
    DatesListNode* current;
    DatesListNode* previous;
    previous = head;
    current = head;
    while (current != NULL) {
        previous = current;
        current = current->next;
        delete previous;
    }
}

void DatesList::deleteNode(string date) //gia diagrafi enos sygkekrimenou date
{
    DatesListNode* previous;
    DatesListNode* current = head;
    while ((current->date) != date && current != NULL) {
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

DatesListNode* DatesList::searchList(string key) { //psaxnei gia sygkekrimeno key
    DatesListNode* current = head;
    while (strcmp(current->date.c_str(),key.c_str())!= 0 && current != NULL) {
        current = current->next;
    }
    if (current == NULL) {
       // cout << "The record you are looking for does not exist" << endl;
        return NULL;
    } else {
        return current;
    }
}



DatesListNode* DatesList::lastNode() // komvou sto telos
{
  

    if (head == NULL) {
      //  head = newnode;
        return head;
    } else {
        DatesListNode* current = head;
        while ((current->next) != NULL) {
            current = (current->next);
        }
        return current;   
    }
    
}

void DatesList::insertNode(string date) //eisagwgi neou komvou sto telos
{
    DatesListNode* newnode = new DatesListNode;
    newnode->next = NULL;
    newnode->date = date;

    if (head == NULL) {
        head = newnode;
    } else {
        DatesListNode* current = head;
        while ((current->next) != NULL) {
            current = (current->next);
        }
        current->next = newnode;
    }
    
    length++;
}

bool DatesList::insertNodeWithCheck(string date) { //eisagwgi me elegxo an yparxei idi
    DatesListNode* newnode = new DatesListNode;
    newnode->next = NULL;
    newnode->date = date;

    if (head == NULL) {
        head = newnode;
        length++;
        return true;
    } else {
        DatesListNode* current = head;

        if (current->date == date) {
            return false;
        }

        while ((current->next) != NULL) {
            if (current->date == date) {
                return false;
            }

            current = (current->next);
        }

        if (current->date == date) {
            return false;
        }

        current->next = newnode;
        length++;
        return true;
    }
}

void DatesList::printList() { //ektypwsi listas
    DatesListNode* current = head;
    while (current != NULL) {
        cout << "id: " << (current->date) << endl;
        current = current->next;
    }
}

void DatesList::destroyRecords() { //katastrofi kapoiou record
    DatesListNode* current = head;
    while (current != NULL) {
        delete current;
        current = current->next;
    }
}

void DatesList::sort() { //taksinomisi
    int length = this->length;

    string * dates = new string[length];

    DatesListNode* current = head;

    for (int i = 0; i < length; i++) {
        dates[i] = current->date;
        current = current->next;
    }

    // sort dates
    for (int i = 0; i < length - 1; i++) {
        for (int j = 0; j < length - i - 1; j++) {
            if (dateToInt(dates[j]) > dateToInt(dates[j + 1])) {
                string temp = dates[j];
                dates[j] = dates[j + 1];
                dates[j + 1] = temp;
            }
        }
    }


    current = head;

    for (int i = 0; i < length; i++) {
        current->date = dates[i];
        current = current->next;
    }

    delete [] dates;
}