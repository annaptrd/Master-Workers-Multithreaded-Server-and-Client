#include "HashTable.h"
#include "hashFunction.h"
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <fstream>

using namespace std;

HashTable::HashTable() : bucketsize(0), h(0), initialized(false), ht(NULL) {
    //cout << "Hashtable created by " << getpid() << endl;
}

HashTable::HashTable(const HashTable & cop) {
   // cout << "Hashtable copy created by " << getpid() << endl;
}

HashTable::~HashTable() {
   // cout << "Hashtable destroyed by " << getpid() << endl;
    if (initialized) {
        BucketNode* temp;
        BucketNode* temp2;
        for (int i = 0; i < h; i++) { //diasxizei to hashtable kai ton katastrefei
            if (this->ht[i] != NULL) {
                temp = ht[i]->next;
                temp2 = ht[i]->next;
                while (temp != NULL) { //katastrefei ola ta bucketnodes gia kathe thesi ston ht
                    temp = temp2->next;
                    delete temp2;
                    temp2 = temp;

                }
                delete this->ht[i];

            }
        }

        if (this->ht) {
            delete [] this->ht;
        }
    }
}

void HashTable::initializeBuckets(int h, int bucketsize) {//arxikopoiei tis theseis tou ht(deiktes se bucketnode)
    cout << "initialized by process: " << getpid() << endl;
    if (initialized == false) {
        this->bucketsize = bucketsize;
        this->h = h;
        this->ht = new BucketNode*[h];
        initialized = true;
        for (int i = 0; i < h; i++) {
            this->ht[i] = NULL;
        }
    }
}

void HashTable::insertWithDisease(Record * record) { //insert gia ht twn astheneiwn
    string key = record->disease;
    insert(key, record);
}

void HashTable::insertWithCountry(Record * record) { //insert gia ht twn xwrwn
    string key = record->country;
    insert(key, record);
}

void HashTable::insert(string key, Record * record) { //eisagwgi ston ht
    unsigned int hash;
    hash = hashFunction(key); //xrisimopoiw ti syanrtisi gia to hashing
    hash = hash % h; //kanw mod h wste na einai mesa sta oria tou pinaka

    if (ht[hash] == NULL) { //an einai to prwto stoixeio pou tha mpei s auti ti thesi
        ht[hash] = new BucketNode(bucketsize);
        ht[hash]->next = NULL;

        Data * d = ht[hash]->bucketptr;
        d[0].text = key; //to onoma tou country/disease
        d[0].empty = false;
        d[0].bst.insertRecord(record);//vazei to record sto dentro autou tou data
        return;
    } else {
        BucketNode* temp = ht[hash]; //pairnei ti thesi tou prwtou bucketnode gi ayti ti thesi tou pinaka
        while (temp != NULL) {
            for (int i = 0; i < bucketsize; i++) {
                Data * d = temp->bucketptr;

                if (d[i].empty != true) { //an den einai adeio kai an to key einai ayto pou theloume mpainei sto dentro autou tou data[i]
                    if (d[i].text == key) {
                        d[i].bst.insertRecord(record);
                        return;
                    }
                }
            }
            temp = temp->next; //an den tairiazei me kanena ap ta data keys aytou tou bucket node paei sto epomeno
        }
        temp = ht[hash]; //an exei ftasei edw simainei oti den ypirxe kapoio me tetoio key opote to vazei twra stin prwti keni thesi tou bucketnode
        for (int i = 0; i < bucketsize; i++) {
            Data * d = temp->bucketptr;

            if (d[i].empty == true) {
                d[i].text = key;
                d[i].bst.insertRecord(record);
                d[i].empty = false;
                return;
            }
        }
        temp = new BucketNode(bucketsize); //to neo bucketnode to bazw stin arxi tis listas twn bucketnodes
        temp->next = ht[hash];
        Data * d = temp->bucketptr;
        d[0].text = key;
        d[0].empty = false;
        d[0].bst.insertRecord(record);
        ht[hash] = temp;
        return;
    }

}

void HashTable::saveToDisk(ofstream &fp) { //gia na ftiaksw to log_file twn workers
    BucketNode* temp;
    int i, j, n = 0;
    string country;
    for (i = 0; i < h; i++) {
        temp = ht[i];
    
        n = 0;
        
        while (temp != NULL) {
            n++;
           
            Data * d = temp->bucketptr;
            
            for (j = 0; j < bucketsize; j++) {
                if (d[j].empty == false) {
                    
                    country=d[j].text;
                    cout<<country<<endl;
                    fp << country; 
                    
                    fp << endl;
                }
            }
            temp = temp->next;
        }
    }
}



void HashTable::printHT() {//ektypwnei ton ht(den xrhsimopoieitai stin ergasia)
    BucketNode* temp;
    int i, j, n = 0;
    for (i = 0; i < h; i++) {
        temp = ht[i];
        cout << "ht[" << i << "] " << endl;
        n = 0;
        while (temp != NULL) {
            n++;
            cout << "bucket :" << n << endl;
            Data * d = temp->bucketptr;
            for (j = 0; j < bucketsize; j++) {
                if (d[j].empty == false) {
                    cout << "data: " << j << " key: " << d[j].text << endl;
                }
            }
            temp = temp->next;
        }
    }
}

Data * HashTable::searchWithDisease(string disease) {
    return search(disease);
}

Data * HashTable::searchWithCountry(string country) {
    return search(country);
}

Data * HashTable::search(string key) { //psaxnei an yparxei kapoio sygkekrimeno stoixeio ston ht k an yparxei epistrefei deikti s ayto to data
    unsigned int hash;
    int i;
    hash = hashFunction(key);
    hash = hash % h;
    if (ht[hash] == NULL) {
        cout << "doesn't exist" << endl;
        return NULL;
    }
    BucketNode* temp;
    temp = ht[hash];
    Data * d = temp->bucketptr;
    while (temp != NULL) {
        for (i = 0; i < bucketsize; i++) {
            if (d[i].empty == false) {
                if (d[i].text == key) {
                  //  cout << "vrethike to: " << d[i].text << " " << key << endl;
                    return &d[i];
                }
            } else {
                break;
            }

        }
        temp = temp->next;
    }
    cout << "doesn't exist" << endl;
    return NULL;

}

void HashTable::printDiseasesStats() {
    BucketNode* temp;
    int i, j; 
    for (i = 0; i < h; i++) {
        temp = ht[i];

        while (temp != NULL) {

            Data * d = temp->bucketptr;
            for (j = 0; j < bucketsize; j++) {
                if (d[j].empty == false) {
                    cout << d[j].text << " ";
                    cout << d[j].bst.activePatients  << endl; //ektypwnei to plithos twn energwn krousmatwn
                }
            }
            temp = temp->next;
        }
    }
}

void HashTable::printGlobalStatsWithDates(int d1,int d2) {
    BucketNode* temp;
    int i, j; 
    for (i = 0; i < h; i++) {
        temp = ht[i];

        while (temp != NULL) {

            Data * d = temp->bucketptr;
            for (j = 0; j < bucketsize; j++) {
                if (d[j].empty == false) {
                    cout << d[j].text << " ";
                    cout<<d[j].bst.searchTreeForDates(d1,d2,NULL)<<endl;;//to plithos twn krousmatwn gia sygkekrimenes imerominies
                }
            }
            temp = temp->next;
        }
    }
}


void HashTable::printGlobalStats() {
    BucketNode* temp;
    int i, j; 
    for (i = 0; i < h; i++) {
        temp = ht[i];

        while (temp != NULL) {

            Data * d = temp->bucketptr;
            for (j = 0; j < bucketsize; j++) {
                if (d[j].empty == false) {
                    cout << d[j].text << " ";
                    cout << d[j].bst.totalCount  << endl; // to plithos olwn twn krousmatwn 
                }
            }
            temp = temp->next;
        }
    }
}