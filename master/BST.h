#ifndef BST_H
#define BST_H
#include "Record.h"
#include "RecordsList.h"

#include <string>

using namespace std;

class TreeNode {
public:
    TreeNode();
    ~TreeNode();
    TreeNode* leftChild;
    TreeNode* rightChild;
    TreeNode* parent;
    TreeNode* grandparent;
    TreeNode* uncle;
    LinkedList* dataList; //deiktis sto head tis listas twn asthenwn aytis tis entrydate
    int count ; //to count asthenwn kathe komvou diladi kathe imerominias
    int date;
    string colour;
    
};

class BST {
public:
    BST();
    ~BST();

    void insertRecord(Record*);
    int searchTreeForDates(int, int, string * c);
    int searchTreeForDatesForAdm(int, int, string);
    int searchTreeForDatesForDis(int, int, string);
    int searchTreeForDatesWithAge(int, int, string ,int,int);
    void searchWithAge(TreeNode* , int *, int, int, string,int, int); 
    void search(TreeNode*, int*, int, int, string * c);
    void searchAdm(TreeNode* , int *, int, int, string );
    void searchDis(TreeNode* , int *, int, int, string );
    void destruct(TreeNode*);
    void balanceTree(TreeNode*);
    void printTree(TreeNode*);
    TreeNode* root;
    int nodes; 
    int totalCount;//synoliko plithos asthenwn gia olo to dentro diladi oles tis imerominies
    int activePatients;//energa krousmaata
    TreeNode* getParent(TreeNode*);
    TreeNode* getGrandparent(TreeNode*); 
    TreeNode* getSibling(TreeNode*);
    TreeNode* getUncle(TreeNode*);
private:
    bool balanced; //an einai zygismeno i oxi
    TreeNode* insert(Record*);
    void rotateLeft(TreeNode*);
    void rotateRight(TreeNode*);
    void balance1(TreeNode*);
    void balance2(TreeNode*);
    void balance3(TreeNode*);
    void balance4(TreeNode*);
    void balance4pt2(TreeNode*);
};

#endif /* BST_H */