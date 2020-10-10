#ifndef COMMANDS_LINKEDLIST_H
#define COMMANDS_LINKEDLIST_H

#include <string>
#include <fstream>

using namespace std;

class Command {
public:
    string query;
    int worker_id;
    string filesystem_path;

    Command(string query) : query(query){

    }

};

class CommandsNode {
public:
    Command * command;
    CommandsNode* next;
};

class CommandsLinkedList {
public:
    CommandsLinkedList();
    ~CommandsLinkedList();
    void insertNode(Command *);
    bool insertNodeWithCheck(Command *);
    void deleteNode(string); 
    CommandsNode* searchList(string);
    void printList(bool printpid = false);
    void printList(int worker_id);
    void printListDetails();
    void saveToDisk(ofstream & fp);
    void destroyCommands();
    int searchForCommand(string c);
    CommandsNode* head;
    int length = 0;
};



#endif //MYCLASSES_Η