#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <string>
#include  <sys/types.h>
#include <sys/socket.h>      /* sockets */
#include <netinet/in.h>      /* internet sockets */
#include <netdb.h>          /* gethostbyaddr */
#include  <dirent.h>
#include  <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <arpa/inet.h> 
#include  "dateToInt.h"
#include "BucketNode.h"
#include "methods.h"
#include "inbetweenDates.h"
#include "BST.h"
#include "CountriesList.h"
#include "DatesList.h"
#include "ChildParameters.h"
using namespace std;

int worker_socket;

void catchinterrupt_worker(int signo) { // SIGINT+SIGQUIT
    printf("\nCatching SIGINT: signo=%d\n", signo);
    writeLogWorker();
    cout << "Bye Worker." << endl;

    close(worker_socket);
    exit(0);
}

void catchinterrupt_refresh_worker(int signo) { // SIGUSR1
    printf("\nCatching SIGINT: signo=%d\n", signo);

    int newFiles = 0;
    ChildParameters* chpar;
    chpar = getChildParams();

    string inputdir = (*chpar).inputdir;

    string path;
    DatesListNode* found;

    CountriesLinkedList* countries;
    countries = getCountriesL();


    CountriesNode* current = countries->head;
    while (current != NULL) {

        DIR *dir_ptr;
        struct dirent *direntp;

        if ((dir_ptr = opendir(current->rec->filesystem_path.c_str())) == NULL) {
            fprintf(stderr, "cannot open %s \n", current->rec->filesystem_path.c_str());


        } else {
            while ((direntp = readdir(dir_ptr)) != NULL) {
                if (*direntp->d_name != '.') {//an to date den einai '.'
                    found = current->rec->dates.searchList(direntp->d_name);

                    if (found == NULL) { //an de vrethike simainei oti einai neo date
                        cout << direntp->d_name << endl;
                        current->rec->dates.insertNode(direntp->d_name);
                        path = current->rec->filesystem_path + "/" + direntp->d_name;
                        ifstream recordsfile(path.c_str()); //anoigei to arxeio gia ta records

                        if (recordsfile.is_open() && recordsfile.good()) {
                            string line = "";
                            int l = 0;
                            while (getline(recordsfile, line)) {
                                stringstream command(line);
                                string parameter;
                                string names[8] = {"ID", "firstname", "lastname", "disease", "country", "entrydate", "exitdate", "age"};
                                string data[8] = {"", "", "", "", "", "", ""};
                                command >> data[0] >> parameter >> data[1] >> data[2] >> data[3] >> data[7]; // >> data[5] >> data[6] >> data[7];

                                data[4] = current->rec->name;
                                data[5] = direntp->d_name;
                                data[6] = "-";

                                for (int i = 0; i < 8; i++) {
                                    if (data[i] == "") {

                                        cout << names[i] << " is missing in line " << l << endl;
                                        cout << "cannot proceed with incorrect file" << endl;

                                    }
                                }

                                Record * record = new Record(data);

                                if (parameter == "ENTER") {

                                    insertPatientRecord(false, record);

                                } else {
                                    recordPatientExit(false, data[0], direntp->d_name);

                                }
                                l++;
                            }
                            workerTotal();
                        } else {
                            cout << "Failed to open file..";

                        }

                        newFiles++;
                    }

                }
            }

            current->rec->dates.sort();
            closedir(dir_ptr);
        }

        current = current->next;
    }
    cout << newFiles << " new files detected and added" << endl;
    exit(0);
}

int main_worker(ChildParameters parameters) {
    int worker_port, newsock;
    struct sockaddr_in server, client;
    socklen_t clientlen, serverlen;
    struct sockaddr *serverptr = (struct sockaddr *) &server;
    struct sockaddr *clientptr = (struct sockaddr *) &client;
    string server_ip, worker_ip;
    int h1 = 500, h2 = 500, b = 3000, server_port;
    workerTotal();
    int bucketsize = (b - sizeof (BucketNode*) - sizeof (Data*)) / (sizeof (Data));
    bool messages = true;
    char hostbuffer[256];
    struct hostent *host_entry;

    setChildParams(&parameters);
    cout << "cc" << endl;
    printf("b = %d \n", b);
    printf("h1 = %d \n", h1);
    printf("h2 = %d \n", h2);
    printf("bucketsize = %d \n", bucketsize);

    int r_fd = open(parameters.s_name.c_str(), O_RDONLY);
    int s_fd = open(parameters.r_name.c_str(), O_WRONLY);

    string inputdir = parameters.inputdir;

    CountriesLinkedList * countries = new CountriesLinkedList();

    //diavazei tis xwres pou tou anatethikan
    char * packet = new char[parameters.bufferSize];

    while (true) {
        read(r_fd, packet, parameters.bufferSize);

        if (packet[0] == '-') {
            break;
        } else {
            string name = packet;
            countries->insertNode(new Country(name, inputdir + "/" + name));

        }
    }
    delete [] packet;

    countries->printListDetails();
    setCountriesL(countries); //aplws setter 


    // read ip and port
    packet = new char[parameters.bufferSize];

    read(r_fd, packet, parameters.bufferSize);

    server_ip = packet;

    read(r_fd, packet, parameters.bufferSize);

    server_port = atoi(packet);

    delete [] packet;




    initializeHashtables(h1, h2, bucketsize); //arxikopoiisi

    CountriesNode* current = countries->head;
    while (current != NULL) {

        DIR *dir_ptr;
        struct dirent *direntp;

        if ((dir_ptr = opendir(current->rec->filesystem_path.c_str())) == NULL) {
            fprintf(stderr, "cannot open %s \n", current->rec->filesystem_path.c_str());

            workerErrors();
            workerTotal();
            return -1;
        } else {
            workerTotal();
            while ((direntp = readdir(dir_ptr)) != NULL) {
                if (*direntp->d_name != '.') {
                    current->rec->dates.insertNode(direntp->d_name);

                }//vazei ola ta dates
            }//se kathe xwra

            current->rec->dates.sort();
            closedir(dir_ptr);
        }

        current = current->next;
    }

    current = countries->head;
    while (current != NULL) {
//        cout << "Country: " << current->rec->name << " has the following dates: " << endl;

        DatesListNode* currentDate = current->rec->dates.head;
        while (currentDate != NULL) {
//            cout << " - " << currentDate->date << endl;

            string path = current->rec->filesystem_path + "/" + currentDate->date;

            ifstream recordsfile(path.c_str()); //anoigei to arxeio gia na parei ta records

            if (recordsfile.is_open() && recordsfile.good()) {
                string line = "";
                int l = 0;
                while (getline(recordsfile, line)) {
                    stringstream command(line);
                    string parameter;
                    string names[8] = {"ID", "firstname", "lastname", "disease", "country", "entrydate", "exitdate", "age"};
                    string data[8] = {"", "", "", "", "", "", ""};
                    command >> data[0] >> parameter >> data[1] >> data[2] >> data[3] >> data[7]; // >> data[5] >> data[6] >> data[7];

                    data[4] = current->rec->name;
                    data[5] = currentDate->date;
                    data[6] = "-";

                    for (int i = 0; i < 8; i++) {
                        if (data[i] == "") {
                            workerTotal();
                            workerErrors();
                            cout << names[i] << " is missing in line " << l << endl;
                            cout << "cannot proceed with incorrect file" << endl;

                            return 0;
                        }
                    }

                    Record * record = new Record(data);

                    if (parameter == "ENTER") {

                        insertPatientRecord(false, record); //eisagwgi twn records apo to arxeio

                    } else {
                        recordPatientExit(false, data[0], currentDate->date);

                    }
                    l++;
                }
                workerTotal();
            } else {
                cout << "Failed to open file..";

                workerTotal();
                workerErrors();
            }

            currentDate = currentDate->next;
        }

        current = current->next;
    }

    //STATS
        current = countries->head;
        DatesListNode* currentDate;
        while (current != NULL) {
            currentDate = current->rec->dates.head;
            while (currentDate != NULL) {
                summaryStatistics(currentDate->date, current->rec->name);
                currentDate = currentDate->next;
            }
            current = current->next;
        }
    

    static struct sigaction act;
    act.sa_handler = catchinterrupt_worker;
    sigfillset(&(act.sa_mask));
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);

    static struct sigaction act2;
    act2.sa_handler = catchinterrupt_worker;
    sigfillset(&(act.sa_mask));
    sigaction(SIGUSR1, &act2, NULL);


    string line = "";


    bool error;

    packet = new char[parameters.bufferSize];


    if ((worker_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    server.sin_family = AF_INET; /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(0); /* The given port */

    if (bind(worker_socket, serverptr, sizeof (server)) < 0) {
        perror("bind");
        return -1;
    }

    if (listen(worker_socket, 7265) < 0) {
        perror("listen");
        return -1;
    }

    serverlen = sizeof (struct sockaddr);

    if (getsockname(worker_socket, serverptr, &serverlen) < 0) { //get port
        perror("getsockname");
        return -1;
    }

    worker_port = ntohs(server.sin_port);



    int hostname = gethostname(hostbuffer, sizeof (hostbuffer));

    if (hostname < 0) {
        perror("gethostname");
        return -1;
    }

    host_entry = gethostbyname(hostbuffer);

    if (host_entry == NULL) {
        perror("gethostbyname");
        return -1;
    }

    worker_ip = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));

    parameters.bufferSize = 5000;

    // connect to server

    struct hostent *rem;

    int fd;

    if ((fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    if ((rem = gethostbyname(server_ip.c_str())) == NULL) {
        herror("gethostbyname");
        exit(1);
    }

    server.sin_family = AF_INET; /* Internet domain */
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(server_port); /* Server port */

    if (connect(fd, serverptr, sizeof (server)) < 0) {
        perror("connect");
        exit(1);
    }

    packet = new char[parameters.bufferSize];

    cout << "Sending IP, PORT and countries to server ... \n";

    strcpy(packet, worker_ip.c_str());
    write(fd, packet, parameters.bufferSize);

    sprintf(packet, "%d", worker_port);
    write(fd, packet, parameters.bufferSize);

    current = countries->head;
    while (current != NULL) {
        strcpy(packet, current->rec->name.c_str());
        write(fd, packet, parameters.bufferSize);
        current = current->next;
    }
    strcpy(packet, "-");
    write(fd, packet, parameters.bufferSize);

    delete [] packet;

    close(fd);

    
    
    printf("Worker %d packet created, bucketsize:%d server ip: %s, server port :%d, worker ip: %s worker port:%d \n", getpid(), bucketsize, server_ip.c_str(), server_port, worker_ip.c_str(), worker_port);
    
    parameters.bufferSize = 5000;
    
    packet = new char[parameters.bufferSize];
    
    while (true) {
        error = false;

        if ((newsock = accept(worker_socket, clientptr, &clientlen)) < 0) {
            perror("accept");
            return -1;
        }

        r_fd = newsock;
        s_fd = newsock;

        if (read(r_fd, packet, parameters.bufferSize) <= 0) {
            break;
        }

        line = packet;

        if (line != "") { //dexetai entoles
            stringstream command(line);
            string entoli;

            string names[8] = {"ID", "firstname", "lastname", "disease", "country", "entrydate", "exitdate"};
            string data[8] = {"", "", "", "", "", "", "", ""};

            command >> data[0] >> data[1] >> data[2] >> data[3] >> data[4] >> data[5] >> data[6] >> data[7];

            if (data[0] == "listCountries" || data[0] == "/listCountries") {
                countries->printListDetails();
                workerTotal();


            } else if (data[0] == "insertPatientRecord" || data[0] == "/insertPatientRecord") {
                for (int i = 1; i < 7; i++) {
                    if (data[i] == "") {
                        cout << names[i] << " is missing " << endl;
                        cout << "cannot proceed with incorrect file" << endl;
                        error = true;
                        workerTotal();
                        workerErrors();
                    }
                }
                if (!error) {
                    if (data[7] == "") {
                        data[7] = "-";
                    }
                    string data2[7] = {data[1], data[2], data[3], data[4], data[5], data[6], data[7]};
                    Record * record = new Record(data2);
                    insertPatientRecord(messages, record);
                    workerTotal();
                }
            } else if (data[0] == "/globalDiseaseStats" || data[0] == "globalDiseaseStats") {
                if ((data[1] == "" && data[2] != "") || (data[1] != "" && data[2] == "")) {
                    cout << "error: You have to enter either two dates or no dates at all" << endl;
                    workerTotal();
                    workerErrors();

                } else {
                    workerTotal();

                    if (data[1] == "") {
                        globalDiseaseStats(messages, NULL, NULL);
                    } else {
                        globalDiseaseStats(messages, &data[1], &data[2]);
                    }
                }
            } else if (data[0] == "/searchPatientRecord" || data[0] == "searchPatientRecord") {
                if (data[1] == "") {
                    workerTotal();
                    workerErrors();
                    cout << "error: You have to enter a record ID" << endl;

                } else {//me select
                    bool found;
                    OutputParameters output(r_fd, s_fd, parameters.bufferSize);
                    searchPatientRecord(messages, data[1], output, &found);
                    workerTotal();



                }
            } else if (data[0] == "/diseaseFrequency" || data[0] == "diseaseFrequency") {
                if (data[1] == "" || data[3] == "" || data[2] == "") {
                    workerTotal();
                    workerErrors();
                    cout << "error: you have to enter one virus name,(maybe a country-optional) and two dates" << endl;

                } else {

                    workerTotal();
                    if (data[4] == "") {//select
                        OutputParameters output(r_fd, s_fd, parameters.bufferSize);
                        diseaseFrequency(messages, data[1], NULL, data[2], data[3], output);


                    } else {
                        //gia sygkekrimeno country
                        OutputParameters output(r_fd, s_fd, parameters.bufferSize);
                        diseaseFrequency(messages, data[1], &data[4], data[3], data[2], output);

                        char * packet = new char[parameters.bufferSize];
                        strcpy(packet, "-");
                        write(s_fd, packet, parameters.bufferSize);
                        delete [] packet;
                    }
                }

            } else if (data[0] == "/topk-AgeRanges" || data[0] == "topk-AgeRanges") {
                if (data[1] == "" || data[2] == "" || (data[3] == "" && data[4] != "") || (data[3] != "" && data[4] == "")) {
                    cout << "error: you have to enter a number(top k), a country ,a disease and two dates" << endl;
                    workerTotal();
                    workerErrors();

                } else {
                    workerTotal();

                    OutputParameters output(r_fd, s_fd, parameters.bufferSize);
                    topk_AgeRanges(messages, atoi(data[1].c_str()), data[2], data[3], data[4], data[5], output);

                    char * packet = new char[parameters.bufferSize];
                    strcpy(packet, "-");
                    write(s_fd, packet, parameters.bufferSize);
                    delete [] packet;

                }
            } else if (data[0] == "/numPatientAdmissions" || data[0] == "numPatientAdmissions") {
                if (data[1] == "" || data[2] == "" || data[3] == "") {
                    workerTotal();
                    workerErrors();

                    cout << "error: you have to enter one virus name,two dates and (maybe a country-optional)" << endl;
                } else {
                    workerTotal();
                    if (data[4] == "") {//select
                        OutputParameters output(r_fd, s_fd, parameters.bufferSize);
                        numPatientAdmissions(messages, data[1], data[2], data[3], NULL, output);



                    } else {
                        //gia sygkekrimeno country
                        OutputParameters output(r_fd, s_fd, parameters.bufferSize);
                        numPatientAdmissions(messages, data[1], data[2], data[3], &data[4], output);
                        char * packet = new char[parameters.bufferSize];
                        strcpy(packet, "-");
                        write(s_fd, packet, parameters.bufferSize);
                        delete [] packet;

                    }
                }
            } else if (data[0] == "/numPatientDischarges" || data[0] == "numPatientDischarges") {
                if (data[1] == "" || data[2] == "" || data[3] == "") {
                    workerTotal();
                    workerErrors();

                    cout << "error: you have to enter one virus name,two dates and (maybe a country-optional)" << endl;
                } else {
                    workerTotal();
                    if (data[4] == "") {
                        OutputParameters output(r_fd, s_fd, parameters.bufferSize);
                        numPatientDischarges(messages, data[1], data[2], data[3], NULL, output);
                    }
                    else {
                        OutputParameters output(r_fd, s_fd, parameters.bufferSize);
                        numPatientDischarges(messages, data[1], data[2], data[3], &data[4], output);
                        char * packet = new char[parameters.bufferSize];
                        strcpy(packet, "-");
                        write(s_fd, packet, parameters.bufferSize);
                        delete [] packet;

                    }

                }

            } else if (data[0] == "/recordPatientExit" || data[0] == "recordPatientExit") {
                if (data[1] == "" || data[2] == "") {

                    cout << "error:you have to enter recordId and exitdate" << endl;
                    workerTotal();
                    workerErrors();
                } else {
                    workerTotal();

                    recordPatientExit(messages, data[1], data[2]);
                }
            } else if (data[0] == "/numCurrentPatients" || data[0] == "numCurrentPatients") {
                if (data[1] == "") {
                    workerTotal();
                    workerErrors();
                    numCurrentPatients(messages, NULL);
                } else {
                    workerTotal();

                    numCurrentPatients(messages, &data[1]);
                }
               
            } else {
                cout << "Invalid Command" << endl;
                workerTotal();
                workerErrors();
                //    structures.errors++;
                cout << "You can use one of the following:" << endl;
                cout << "exit" << endl;
                cout << "diseaseFrequency" << endl;
                cout << "topk-AgeRanges" << endl;
                cout << "numPatientAdmissions" << endl;
                cout << "numPatientDischarges" << endl;
                cout << "listCountries" << endl;
                cout << "searchPatientRecord" << endl;

            }
        }
    }

    delete [] packet;


    close(s_fd);


    return 0;
}
