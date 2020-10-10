#include <iostream>
#include "CommandsList.h"
#include <cstdlib>

using namespace std;

CommandsLinkedList::CommandsLinkedList() {
    //    cout << "list created" << endl;
    head = NULL;
}

CommandsLinkedList::~CommandsLinkedList() { //katastrefei ti lista
    //   cout << "list destructed" << endl;
    CommandsNode* current;
    CommandsNode* previous;
    previous = head;
    current = head;
    while (current != NULL) {
        previous = current;
        current = current->next;
        delete previous;
    }
}

void CommandsLinkedList::deleteNode(string key) //gia diagrafi enos sygkekrimenou country
{
    CommandsNode* previous;
    CommandsNode* current = head;
    while ((current->command->query) != key && current != NULL) {
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

CommandsNode* CommandsLinkedList::searchList(string key) { //psaxnei gia sygkekrimeno country
    CommandsNode* current = head;
    while ((current->command->query) != key && current != NULL) {
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

void CommandsLinkedList::insertNode(Command * com) //eisagwgi neou komvou sto telos
{
    CommandsNode* newnode = new CommandsNode;
    newnode->next = NULL;
    newnode->command = com;

    if (head == NULL) {
        head = newnode;
    } else {
        CommandsNode* current = head;
        while ((current->next) != NULL) {
            current = (current->next);
        }
        current->next = newnode;
    }
}

bool CommandsLinkedList::insertNodeWithCheck(Command * com) { //eisagwgi me elegxo an yparxei idi
    CommandsNode* newnode = new CommandsNode;
    newnode->next = NULL;
    newnode->command = com;

    if (head == NULL) {
        head = newnode;
        return true;
    } else {
        CommandsNode* current = head;

        if (current->command->query == com->query) {
            return false;
        }

        while ((current->next) != NULL) {
            if (current->command->query == com->query) {
                return false;
            }

            current = (current->next);
        }

        if (current->command->query == com->query) {
            return false;
        }

        current->next = newnode;
        return true;
    }
}

void CommandsLinkedList::printList(bool printpid) { //ektypwsi listas
    CommandsNode* current = head;
    while (current != NULL) {
        cout << "name: " << (current->command->query);
        if (printpid) {
            cout << " pid: " /*<< current->command->worker_pid*/ << endl;
        } else {
            cout << endl;
        }
        current = current->next;
    }
}

void CommandsLinkedList::printList(int worker_id) { //ektypwsi listas
    CommandsNode* current = head;
    while (current != NULL) {
        if (current->command->worker_id == worker_id) {
            cout << " tid: " << pthread_self() << " :"  << current->command->query << endl;
        }
        current = current->next;
    }
}


void CommandsLinkedList::printListDetails() { //ektypwsi listas
    CommandsNode* current = head;
    while (current != NULL) {
        cout << /* "name: " <<*/ (current->command->query);
        cout << /* " pid: "*/" " /*<< current->rec->worker_pid*/;
        //cout << " path: " << current->rec->filesystem_path;
        cout << endl;
        current = current->next;
    }
}

void CommandsLinkedList::saveToDisk(ofstream & fp) { //grafei sto disko 
    CommandsNode* current = head;
    while (current != NULL) {
        fp << /* "name: " <<*/ (current->command->query);
        //fp << /* " pid: "*/" " << current->rec->worker_pid;
        fp << endl;
        current = current->next;
    }
}

int CommandsLinkedList::searchForCommand(string c) { //anazitisi gia sygkekrimeni country
    int count = 0;
    CommandsNode* current = head;

    while (current != NULL) {
        if (current->command->query == c) {
            count++;
        }
        current = current->next;
    }

    return count;
}

void CommandsLinkedList::destroyCommands() { //katastrofi kapoiou record
    CommandsNode* current = head;
    while (current != NULL) {
        delete current->command;
        current = current->next;
    }
}

