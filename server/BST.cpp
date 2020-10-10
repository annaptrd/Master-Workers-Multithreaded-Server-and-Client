#include "BST.h"
#include <iostream>
#include "dateToInt.h"
#include "inbetweenDates.h"
#include <cstdlib>
#include <cstdlib>
#include "RecordsList.h"

using namespace std;

TreeNode* BST::getParent(TreeNode* n) { //epistrefei to gonea 
    if (n == NULL)
        return NULL;
    else {
        return n->parent;
    }

}

TreeNode* BST::getGrandparent(TreeNode* n) { //epistrefei ton pappou
    return getParent(getParent(n));
}

TreeNode* BST::getSibling(TreeNode* n) { //epistrefei ton adelfo 
    TreeNode* p = getParent(n);

    if (p == NULL) {
        return NULL;
    }
    if (n == p->leftChild) {
        return p->rightChild;
    } else {
        return p->leftChild;
    }
}

TreeNode* BST::getUncle(TreeNode* n) { //epistrefei to theio
    TreeNode* p = getParent(n);
    return getSibling(p);
}

BST::BST() {
    nodes = 0;
    totalCount = 0;
    activePatients = 0; //energa krousmata
    root = NULL;
    balanced = true;
}

void BST::printTree(TreeNode* n) { //ektypwsi me apli dfs
    if (n == NULL)
        return;
    printTree(n->leftChild);
    printTree(n->rightChild);
}


TreeNode::TreeNode() {
    rightChild = NULL;
    leftChild = NULL;
    parent = NULL;
    grandparent = NULL;
    uncle = NULL;
    count = 0;
    dataList = NULL;
    colour = "red";
}

void BST::rotateRight(TreeNode* n) { //ta aparaitita rotations gia to balancing
    TreeNode* newnode = n->leftChild;
    TreeNode* par = getParent(n);
    if (newnode == NULL) {
        cout << "error" << endl;
        exit(1);
    }
    n->leftChild = newnode->rightChild;
    newnode->rightChild = n;
    n->parent = newnode;
    if (n->leftChild != NULL) {
        n->leftChild->parent = n;
    }

    if (par != NULL) {
        if (n == par->leftChild) {
            par->leftChild = newnode;
        } else if (n == par->rightChild) {
            par->rightChild = newnode;
        }
    }
    newnode->parent = par;
}

void BST::rotateLeft(TreeNode* n) { //ta aparaitita rotations gia to balancing
    TreeNode* newnode = n->rightChild;
    TreeNode* par = getParent(n);
    if (newnode == NULL) {
        cout << "error" << endl;
        exit(1);
    }
    n->rightChild = newnode->leftChild;
    newnode->leftChild = n;
    n->parent = newnode;
    if (n->rightChild != NULL) {
        n->rightChild->parent = n;
    }

    if (par != NULL) {
        if (n == par->leftChild) {
            par->leftChild = newnode;
        } else if (n == par->rightChild) {
            par->rightChild = newnode;
        }
    }
    newnode->parent = par;
}

void BST::balance1(TreeNode* n) { //h 1h periptwsh gia to balancing
    n->colour = "black";
}

void BST::balance2(TreeNode* n) { //h 2h periptwsh gia to balancing
    return;
}

void BST::balance3(TreeNode* n) { ////h 3h periptwsh gia to balancing
    getParent(n)->colour = "black";
    getUncle(n)->colour = "black";
    getGrandparent(n)->colour = "red";
    balanceTree(getGrandparent(n));
}

void BST::balance4pt2(TreeNode* n) { //h 4h periptwsh gia to balancing meros deytero
    TreeNode* p = getParent(n);
    TreeNode* g = getGrandparent(n);

    if (n == p->leftChild) {
        rotateRight(g);
    } else {
        rotateLeft(g);
    }
    p->colour = "black";
    g->colour = "red";
}

void BST::balance4(TreeNode* n) { //h 4h periptwsh gia to balancing meros prwto
    TreeNode* p = getParent(n);
    TreeNode* g = getGrandparent(n);

    if (n == p->rightChild && p == g->leftChild) {
        rotateLeft(p);
        n = n->leftChild;
    } else if (n == p->leftChild && p == g->rightChild) {
        rotateRight(p);
        n = n->rightChild;
    }
    balance4pt2(n);
}

void BST::balanceTree(TreeNode* n) { //to balancing pou kalei tis antistoixes synartiseis gia tis 4 periptwseis
    if (getParent(n) == NULL) {
        balance1(n);
    } else if (getParent(n)->colour == "black") {
        balance2(n);
    } else if (getUncle(n) != NULL && getUncle(n)->colour == "red") {
        balance3(n);
    } else {
        balance4(n);
    }

}

void BST::insertRecord(Record* r) { // kalei tin insert gia tin eisagwgi k meta ti balancetree gia to balancing
    TreeNode* t;
    t = insert(r);
    if (r->exitdate == NO_DATE) {
        activePatients++;
    }

    if (balanced) {
        balanceTree(t);
        root = t;
        while (root->parent != NULL) {
            root = root->parent;
        }
    }
}

TreeNode* BST::insert(Record* r) {

    int key = r->entrydate;
    if (root == NULL) { //an den yparxei root, dimiourgise to afou einai o prwtos komvos
        TreeNode* newNode = new TreeNode;
        (newNode->dataList) = new LinkedList;
        newNode->dataList->insertNode(r);
        newNode->date = key;
        root = newNode;
        (root->count)++; //count gia auti tin imerominia
        totalCount++;
        return newNode;
    } else {
        TreeNode* current;
        current = root;
        while (true) {//alliws afou yparxei root diasxizw to dentro analoga me to key
            if (key > (current->date)) { //an to key einai megalytero tis imerominias aytou tou paidiou, proxwraei deksia
                if ((current->rightChild) == NULL) {//an to paidi ayto den yparxei to dimiourgw
                    TreeNode* newNode = new TreeNode;
                    (newNode->dataList) = new LinkedList;
                    newNode->dataList->insertNode(r); //eisagei to stoixeio sti lista tou paidiou me ayti tin imerominia
                    newNode->date = key;
                    (newNode->count)++;
                    totalCount++;

                    newNode->parent = current;
                    newNode->grandparent = current->parent;  //enimerwnw tis times aytwn olwn 
                    if (newNode->parent != root) {
                        if ((newNode->parent) == (newNode->grandparent->leftChild)) {
                            newNode->uncle = (newNode->grandparent->rightChild);
                        } else {
                            newNode->uncle = (newNode->grandparent->leftChild);

                        }
                    }

                    (current->rightChild) = newNode;
                    return newNode;
                }
                current = current->rightChild; //an de vrethike, proxwraw sto epomeno 
            } else if (key < (current->date)) { //an to key einai mikrotero tis imerominias aytou tou paidiou, proxwraei aristera
                if ((current->leftChild) == NULL) {//omoiws me  apo panw alla gi aristera
                    TreeNode* newNode = new TreeNode;
                    (newNode->dataList) = new LinkedList;
                    newNode->dataList->insertNode(r);
                    newNode->date = key;
                    (newNode->count)++;
                    totalCount++;

                    newNode->parent = current;
                    newNode->grandparent = current->parent; 
                    if (newNode->parent != root) {
                        if ((newNode->parent) == (newNode->grandparent->leftChild)) {
                            newNode->uncle = (newNode->grandparent->rightChild);
                        } else {
                            newNode->uncle = (newNode->grandparent->leftChild);

                        }
                    }
                    (current->leftChild) = newNode;

                    return newNode;
                }
                current = current->leftChild;
            } else { //otan tha einai iso me to date enos komvou mpainei sti lista aytou tou komvou
                current->dataList->insertNode(r);
                (current->count)++;
                totalCount++;
                return current;
            }
        }
    }
}

void BST::search(TreeNode* current, int *tot, int date1, int date2, string * c) {
    if (current == NULL) {
        return;
    }
    if (inbetweenDates(current->date, date1, date2)) { //epistrefei an einai anamesa stis dyo imerominies
        if (c == NULL) {
            *tot = *tot + current->count;
        } else {
            *tot = *tot + current->dataList->searchForCountry(*c);
        }
    }
    search(current->leftChild, tot, date1, date2, c);

    search(current->rightChild, tot, date1, date2, c);
}


void BST::searchDis(TreeNode* current, int *tot, int date1, int date2, string v) {
    if (current == NULL) {
        return;
    }
    Node* curr;
    curr=current->dataList->head;
    while(curr!=NULL){
        
        if (inbetweenDates(curr->rec->exitdate, date1, date2)) { //epistrefei an einai anamesa stis dyo imerominies     
            if(curr->rec->disease==v)
                *tot = *tot + 1;       
        }
        curr=curr->next;
    }
    
    searchDis(current->leftChild, tot, date1, date2, v);

    searchDis(current->rightChild, tot, date1, date2, v);
}

void BST::searchAdm(TreeNode* current, int *tot, int date1, int date2, string v) {
    if (current == NULL) {
        return;
    }
    if (inbetweenDates(current->date, date1, date2)) { //epistrefei an einai anamesa stis dyo imerominies     
            *tot = *tot + current->dataList->searchForDisease(v);       
    }
    searchAdm(current->leftChild, tot, date1, date2, v);

    searchAdm(current->rightChild, tot, date1, date2, v);
}

void BST::searchWithAge(TreeNode* current, int *tot, int date1, int date2, string c,int a1, int a2) {
    if (current == NULL) {
        return;
    }
    if (inbetweenDates(current->date, date1, date2)) { //epistrefei an einai anamesa stis dyo imerominies
            *tot = *tot + current->dataList->searchForCountryWithAge(c,a1,a2);
    }
    
    searchWithAge(current->leftChild, tot, date1, date2, c,a1,a2);

    searchWithAge(current->rightChild, tot, date1, date2, c,a1,a2);
}

int BST::searchTreeForDates(int d1, int d2, string * c) { //epistrefei posa atoma eixan entrydate anamesa s ayta ta dyo dates
    TreeNode* current = root;    
    int total = 0;
    search(current, &total, d1, d2, c);
    return total;
}

int BST::searchTreeForDatesForAdm(int d1, int d2, string  v) { //epistrefei posa atoma eixan entrydate anamesa s ayta ta dyo dates
    TreeNode* current = root;    
    int total = 0;
    searchAdm(current, &total, d1, d2, v);
    return total;
}

int BST::searchTreeForDatesForDis(int d1, int d2, string  v) { //epistrefei posa atoma eixan entrydate anamesa s ayta ta dyo dates
    TreeNode* current = root;    
    int total = 0;
    searchDis(current, &total, d1, d2, v);
    return total;
}



int BST::searchTreeForDatesWithAge(int d1, int d2, string c,int age1,int age2) { //epistrefei posa atoma eixan entrydate anamesa s ayta ta dyo dates
    TreeNode* current = root;    
    int total = 0;
    searchWithAge(current, &total, d1, d2, c,age1,age2);
    return total;
}

void BST::destruct(TreeNode* current) { //diasxizei to dentro kai katastrefei kathe paidi

    if (current != NULL) {
        if (current->leftChild != NULL) {
            destruct(current->leftChild);
        }
        if (current->rightChild != NULL) {
            destruct(current->rightChild);
        }

        delete current->dataList;
        delete current;
    }
}

TreeNode::~TreeNode() {
    // cout << "tree node destroyed" << endl;
}

BST::~BST() { //kalei to destruct gia na katastrepsei to dentro
    TreeNode* current;
    current = root;
    destruct(current);
    //cout << "tree destroyed" << endl;
}

