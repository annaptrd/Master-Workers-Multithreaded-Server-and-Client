
#include "methods.h"
#include "RecordsList.h"
#include "HashTable.h"
#include "dateToInt.h"
#include "ChildParameters.h"
#include "CountriesList.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
#include <fstream>
#include <fcntl.h>
#include <cstring>

using namespace std;

class Structures {
public:
    LinkedList list;
    HashTable hashtable1;
    HashTable hashtable2;
    int total;//gia to log file
    int errors;
    ChildParameters* params;
    CountriesLinkedList* countriesL;
};

Structures structures;

void setChildParams(ChildParameters* x){
    structures.params=x;
}

ChildParameters* getChildParams(){
    return structures.params;
}


void setCountriesL(CountriesLinkedList* x){
    structures.countriesL=x;
}

CountriesLinkedList* getCountriesL(){
    return structures.countriesL;
}


void workerErrors(){
    structures.errors++;
}
void workerTotal(){
    structures.total++;
}
void initializeHashtables(int h1, int h2, int b) { //arxikopoiisi
    structures.hashtable1.initializeBuckets(h1, b);
    structures.hashtable2.initializeBuckets(h2, b);
}

void cleanupStructures() { //gia tin katastrofi twn domwn
    structures.list.destroyRecords();
}

void insertPatientRecord(bool messages, Record * record) { //gia tin eisagwgi twn records
    if (messages) {
        //     cout << "method insertPatientRecord" << endl;
    }

    if (structures.list.insertNodeWithCheck(record)) {//eisagwgi sti lista an den yparxei idi
        structures.hashtable1.insertWithDisease(record); //eisagwgi stous ht
        structures.hashtable2.insertWithCountry(record);
        if (messages) {
            cout << "Record added" << endl;
        }
    } else {
        if (messages) {
            cout << "not ok" << endl;
        }
    }
}


void writeLogWorker() {
    string s = string("log_file_") + to_string(getpid()) + ".txt";

    ofstream fp(s.c_str()); 
    
    structures.hashtable2.saveToDisk(fp);
    
    fp << "TOTAL " <<structures.total << endl;
    fp << "SUCCESS " <<structures.total - structures.errors << endl;
    fp << "FAIL " <<structures.errors << endl;
}

void recordPatientExit(bool messages, string id, string exitdate) {//enimerwnei to exitdate an den yparxei
    if (messages) {
        //    cout << "method recordPatientExit" << endl;
    }
    int exdate = dateToInt(exitdate);
    Node * n = structures.list.searchList(id);
    if (n != NULL) {
        Record * record = n->rec;

        if (record->exitdate == NO_DATE) {//an den yparxei exitdate tote to enimerwnw
            // if exitdate is after entry date then:
            if (exdate > record->entrydate) {
                record->setExitDate(exitdate);
                cout << "Record updated" << endl;
                Data * data = structures.hashtable1.searchWithDisease(record->disease);
                (data->bst.activePatients)--;
            } else {//an i exitdate einai prin tin entrydate
                cout << "ERROR:wrong exit date" << endl;
            }
        } else if(record->entrydate==NO_DATE){
            cout<<"ERROR: exit date without entry date"<<endl;}
        else{
            cout << "Record updated" << endl; //yparxei idi vasika aplws gia tis anagkes tou validator...
            // cout << "exit date:already exists" << endl;
        }
    } 
}

void searchPatientRecord(bool messages, string id, OutputParameters & output,bool *found) {//me select
  Node * n = structures.list.searchList(id);
  char * packet = new char[output.bufferSize];
    if (n != NULL) {
        string packet2;
        if (n->rec->exitdate == 0){
           // *found=true;
            //cout << n->rec->id << " " << n->rec->firstname << " " << n->rec->lastname << " " << n->rec->disease << " " << n->rec->age << " " << intToDate(n->rec->entrydate) << " " << "--" << endl;
            packet2=n->rec->id+" "+n->rec->firstname+" "+n->rec->lastname+" "+n->rec->disease+" "+to_string(n->rec->age)+" "+intToDate(n->rec->entrydate)+" --";
            sprintf(packet, "%s", packet2.c_str());
            write(output.s_fd, packet, output.bufferSize);
        }else{
           // *found=true;
            packet2=n->rec->id+" "+n->rec->firstname+" "+n->rec->lastname+" "+n->rec->disease+" "+to_string(n->rec->age)+" "+intToDate(n->rec->entrydate)+" "+intToDate(n->rec->exitdate);
            sprintf(packet, "%s", packet2.c_str());
            write(output.s_fd, packet, output.bufferSize);
           // cout << n->rec->id << " " << n->rec->firstname << " " << n->rec->lastname << " " << n->rec->disease << " " << n->rec->age << " " << intToDate(n->rec->entrydate) << " " << intToDate(n->rec->exitdate) << endl;
    }}else{
        sprintf(packet, "###"); //an de vrethei
        write(output.s_fd, packet, output.bufferSize);
    }
  delete [] packet;
}

void numCurrentPatients(bool messages, string * v) { //ektypwnei ta energa krousmata
    if (v != NULL) {
        //    cout << "method numCurrentPatients" << *v << endl;
    } else {
        //    cout << "method numCurrentPatients" << endl;
    }

    if (v != NULL) {
        Data * data = structures.hashtable1.searchWithDisease(*v); //epistrefei to data me to sygkekrimeno disease
        if (data != NULL) {
            cout << *v;
            cout << " " << data->bst.activePatients << endl; //energa krousmata
        }
    } else {
        structures.hashtable1.printDiseasesStats();
    }

}

void globalDiseaseStats(bool messages, string* d1, string* d2) {
    if ((d1 != NULL && d2 == NULL) || (d1 == NULL && d2 != NULL)) {
        cout << "prepei na dothoun eite dyo imerominies eite kamia" << endl;
        return;
    }
    if (d1 == NULL && d2 == NULL) {//ektypwnei posoi arrwstoi exoun katagrafei genika gia kathe iwsi
        structures.hashtable1.printGlobalStats(); //sto ht twn diseases
    } else {
        int date1 = dateToInt(*d1);
        int date2 = dateToInt(*d2);
        structures.hashtable1.printGlobalStatsWithDates(date1, date2); //ektypwnei posoi arrwstoi exoun katagrafei anamesa s aytes tis imerominies
    }
}

void diseaseFrequency(bool messages, string v, string* c, string d1, string d2, OutputParameters & output) {
    if (c == NULL) {//an den yparxei country
        Data * data = structures.hashtable1.searchWithDisease(v);
        // cout << v;

        if (data != NULL) {//an vrethei i astheneia
            int i1 = dateToInt(d1);
            int i2 = dateToInt(d2);

            int value = data->bst.searchTreeForDates(i1, i2, NULL);
            char * packet = new char[output.bufferSize];
            sprintf(packet, "%d", value);
            write(output.s_fd, packet, output.bufferSize);
            delete [] packet;

//            cout << " " <<  << endl; //ektypwnei posa einai ta krousmata mesa aytes tis imerominies gia ayti tin arrwstia
        }
        return;
    } else {

        Data * data = structures.hashtable1.searchWithDisease(v);
        //cout <<v;

        if (data != NULL) {
            int i1 = dateToInt(d1);
            int i2 = dateToInt(d2);

            int value = data->bst.searchTreeForDates(i1, i2, c);

            char * packet = new char[output.bufferSize];
            sprintf(packet, "%d", value);
            write(output.s_fd, packet, output.bufferSize);
            delete [] packet;

            //            cout <<" "<<data->bst.searchTreeForDates(i1, i2, c) << endl;//omoia alla gia sygkekrimena countries
        }
    }

    return;
}

void numPatientAdmissions(bool messages, string v, string d1, string d2, string* c, OutputParameters & output) {//select
  int i1 = dateToInt(d1);
  int i2 = dateToInt(d2);
   if(c!=NULL){
    Data * data = structures.hashtable2.searchWithCountry(*c);
    if (data != NULL) {
        
        int value = data->bst.searchTreeForDatesForAdm(i1, i2, v); //ektypwnei posa einai ta krousmata mesa aytes tis imerominies gia ayti tin arrwstia
        char * packet = new char[output.bufferSize];
        sprintf(packet, "%s %d", (*c).c_str(), value);
        write(output.s_fd, packet, output.bufferSize);
        delete [] packet;
    } else {
        cout << "Country Not Found" << endl;
    }}
  else{//gia kathe country
      
    BucketNode* temp;
    int i, j, n = 0;
    string country;
    int value;
    string packet2="";
    char * packet = new char[output.bufferSize];
    for (i = 0; i < structures.hashtable2.h;i++) {
        temp = structures.hashtable2.ht[i];
        n = 0;
        
        while (temp != NULL) {
            n++;
           // cout << "bucket :" << n << endl;
            Data * d = temp->bucketptr;
            
            for (j = 0; j < structures.hashtable2.bucketsize; j++) {
                if (d[j].empty == false) {
                    
                    value = d[j].bst.searchTreeForDatesForAdm(i1, i2, v);
                    //cout<<getpid()<<endl;
                    sprintf(packet, "%s %d \n", d[j].text.c_str(), value);
                    packet2=packet2+packet;
                    }
            }
            temp = temp->next;
        } 
    }
    write(output.s_fd, packet2.c_str(), output.bufferSize);
    delete [] packet;
      
  }
}

void numPatientDischarges(bool messages, string v, string d1, string d2, string* c, OutputParameters & output) {//me select
   
    int i1 = dateToInt(d1);
    int i2 = dateToInt(d2);
    if(c!=NULL){
        Data * data = structures.hashtable2.searchWithCountry(*c);
        if (data != NULL) {
        //ektypwnei posa einai ta krousmata mesa aytes tis imerominies gia ayti tin arrwstia
        int value = data->bst.searchTreeForDatesForDis(i1, i2, v); //ektypwnei posa einai ta krousmata mesa aytes tis imerominies gia ayti tin arrwstia
        char * packet = new char[output.bufferSize];
        sprintf(packet, "%s %d", (*c).c_str(), value);
        write(output.s_fd, packet, output.bufferSize);
        delete [] packet;
    } else {
        cout << "Country Not Found" << endl;
    }}
  else{//gia kathe country
      
    BucketNode* temp;
    int i, j, n = 0;
    string country;
    int value;
    string packet2="";
    char * packet = new char[output.bufferSize];
    for (i = 0; i < structures.hashtable2.h;i++) {
        temp = structures.hashtable2.ht[i];
        n = 0;
        
        while (temp != NULL) {
            n++;
           // cout << "bucket :" << n << endl;
            Data * d = temp->bucketptr;
            
            for (j = 0; j < structures.hashtable2.bucketsize; j++) {
                if (d[j].empty == false) {
                    
                    value = d[j].bst.searchTreeForDatesForDis(i1, i2, v);
                    //cout<<getpid()<<endl;
                    sprintf(packet, "%s %d \n", d[j].text.c_str(), value);
                    packet2=packet2+packet;
                    }
            }
            temp = temp->next;
        } 
    }
    write(output.s_fd, packet2.c_str(), output.bufferSize);
    delete [] packet;
      
  }
}

void statistics(string c, string disease, string d1, string d2, int* age_groups) {
    Data* data = structures.hashtable1.searchWithDisease(disease);
    // int age_groups[4];
    if (data != NULL) {
        int i1 = dateToInt(d1);
        int i2 = dateToInt(d2);

        *(age_groups + 0) = data->bst.searchTreeForDatesWithAge(i1, i2, c, 0, 20);
 
        *(age_groups + 1) = data->bst.searchTreeForDatesWithAge(i1, i2, c, 21, 40);
       
        *(age_groups + 2) = data->bst.searchTreeForDatesWithAge(i1, i2, c, 41, 60);
        
        *(age_groups + 3) = data->bst.searchTreeForDatesWithAge(i1, i2, c, 61, 150);
        
    }
    
}


void summaryStatistics(string date, string country,OutputParameters & output ){
 
    BucketNode* temp;
    int i, j, n = 0;
//    int i2, j2, n2 = 0;
    char * packet = new char[output.bufferSize];
    string packet2="";
    string packet3="";
    string packet4="";
    string packetgen="";
    int groups[4];
    
    for (i = 0; i < structures.hashtable1.h;i++) {
        temp = structures.hashtable1.ht[i];
        n = 0;
        
        while (temp != NULL) {
            n++;
            Data * d = temp->bucketptr;
            
            for (j = 0; j < structures.hashtable1.bucketsize; j++) {
                if (d[j].empty == false) {
                        statistics(country,d[j].text,date, date, groups);
                        if((*(groups + 0+*(groups + 1)+*(groups + 2)+*(groups + 3))>0)){ //an yparxei to disease sto country/date ayto
                                packet2 = date+ "\n" + country + "\n";
                                packet3 = "Age Range 0-20 years: " + to_string(*(groups + 0)) + " cases \n";
                                packet4 = "Age Range 21-40 years: " + to_string(*(groups + 1)) + " cases \n";
                                packet4 = packet3 + packet4;
                                packet3 = "Age Range 41-60 years: " + to_string(*(groups + 2)) + " cases \n";
                                packet4 = packet4 + packet3;
                                packet3 = "Age Range 60+ years: " + to_string(*(groups + 0)) + " cases \n";
                                packet4 = packet3 + packet4;
                                packet2 = packet2 + packet4;
                                sprintf(packet, "%s \n", packet2.c_str());
                                packetgen = packetgen + packet2;
                            }
                    }
            }
            temp = temp->next;
        } 
    }
    
    //sprintf(packet, "%s\n", packetgen.c_str());
    write(output.s_fd, packetgen.c_str(), output.bufferSize);
    delete [] packet;
}




void topk_AgeRanges(bool messages, int k, string c, string v, string d1, string d2, OutputParameters & output) {

    int groups[4];
    int g[4];
    string group[4] = {"0-20", "21-40", "41-60", "60+"};

    statistics(c, v, d1, d2, groups);
    g[0] = *(groups + 0);
    g[1] = *(groups + 1);
    g[2] = *(groups + 2);
    g[3] = *(groups + 3);


    int temp, total = 0;
    string tempp;
    int i, j, n = 4;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (g[j] < g[j + 1]) {

                temp = g[j];
                g[j] = g[j + 1];
                g[j + 1] = temp;
                tempp = group[j];
                group[j] = group[j + 1];
                group[j + 1] = tempp;
            }
        }

    }
    for (i = 0; i < n; i++) {
        total = total + g[i];
    }
    float percentage;
    char * packet = new char[output.bufferSize];
    string t;

    string res;
    for (i = 0; i < k; i++) {
        // int value = data->bst.searchTreeForDates(i1, i2, c);


        percentage = (g[i] / float(total))*100;
        //cout<<group[i]<<" "<<percentage <<"%"<<endl;

        // sprintf(packet, "%s ", group[i]);
        //write(output.s_fd, packet, output.bufferSize);
        //sprintf(t, " %f", percentage);
        //std::ostringstream ss;
        // ss << percentage;
        //std::string s(ss.str());
        //string te= percentage;
        // res=group[i] + s;
        sprintf(packet, "%s %f%c", group[i].c_str(), percentage, '%');
        write(output.s_fd, packet, output.bufferSize);
    }
    delete [] packet;

}

void topk_Diseases(bool messages, int k, string c, string* d1, string*d2) {
    if ((d1 != NULL && d2 == NULL) || (d1 == NULL && d2 != NULL)) {
        cout << "prepei na dothoun eite dyo dates eite katholou" << endl;
        return;
    }
}

void topk_Countries(bool messages, int k, string dis, string* d1, string*d2) {
    if ((d1 != NULL && d2 == NULL) || (d1 == NULL && d2 != NULL)) {
        cout << "prepei na dothoun eite dyo dates eite katholou" << endl;
        return;
    }
}

