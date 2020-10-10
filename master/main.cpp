#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <string>
#include  <sys/types.h>
#include  <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <cstring>

#include "BucketNode.h"
#include "inbetweenDates.h"
#include "BST.h"
#include "CountriesList.h"
#include "methods_aggregator.h"

using namespace std;

// "${OUTPUT_PATH}" -i inputdir -w 3 -b 4096

void catchinterrupt_aggregator(int signo) { //ctr+C
    printf("\nCatching SIGINT: signo=%d\n", signo);

    cleanupWorkers();
    cleanupPipes();
    writeLog();

    cout << "Bye." << endl;
    exit(0);
}

int main(int argc, char * argv[]) {
    int w, bufferSize;
    string inputdir;
    string server_ip;
    int server_port;

    if (argc != 11) {
        printf("lathos parametroi \n");
        printf("prepei na einai: -w numWorkers -b bufferSize -s serverIP  -p serverPort -i input_dir \n");
        return 0;
    }

    //elegxos gia orismata
    if (string(argv[1]) == "-w") {
        w = atoi(argv[2]);
    }//h thesh tou h1
    else if (string(argv[3]) == "-w") {
        w = atoi(argv[4]);
    } else if (string(argv[5]) == "-w") {
        w = atoi(argv[6]);
    } else if (string(argv[7]) == "-w") {
        w = atoi(argv[8]);
    } else if (string(argv[9]) == "-w") {
        w = atoi(argv[10]);
    } else {
        printf("lathos parametroi \n");
        printf("prepei na einai: -w numWorkers -b bufferSize -s serverIP  -p serverPort -i input_dir \n");
        return 0;
    }
    if (string(argv[1]) == "-b") {
        bufferSize = atoi(argv[2]);
    } else if (string(argv[3]) == "-b") {
        bufferSize = atoi(argv[4]);
    } else if (string(argv[5]) == "-b") {
        bufferSize = atoi(argv[6]);
    } else if (string(argv[7]) == "-b") {
        bufferSize = atoi(argv[8]);
    } else if (string(argv[9]) == "-b") {
        bufferSize = atoi(argv[10]);
    } else {
        printf("lathos parametroi \n");
        printf("prepei na einai: -w numWorkers -b bufferSize -s serverIP  -p serverPort -i input_dir\n");
        return 0;
    }
    if (string(argv[1]) == "-i") {
        inputdir = (argv[2]);
    } else if (string(argv[3]) == "-i") {
        inputdir = (argv[4]);
    } else if (string(argv[5]) == "-i") {
        inputdir = (argv[6]);
    } else if (string(argv[7]) == "-i") {
        inputdir = (argv[8]);
    } else if (string(argv[9]) == "-i") {
        inputdir = (argv[10]);
    } else {
        printf("lathos parametroi \n");
        printf("prepei na einai: -w numWorkers -b bufferSize -s serverIP  -p serverPort -i input_dir\n");
        return 0;
    }

    if (string(argv[1]) == "-s") {
        server_ip = (argv[2]);
    } else if (string(argv[3]) == "-s") {
        server_ip = (argv[4]);
    } else if (string(argv[5]) == "-s") {
        server_ip = (argv[6]);
    } else if (string(argv[7]) == "-s") {
        server_ip = (argv[8]);
    } else if (string(argv[9]) == "-s") {
        server_ip = (argv[10]);
    } else {
        printf("lathos parametroi \n");
        printf("prepei na einai: -w numWorkers -b bufferSize -s serverIP  -p serverPort -i input_dir \n");
        return 0;
    }

    if (string(argv[1]) == "-p") {
        server_port = atoi(argv[2]);
    } else if (string(argv[3]) == "-p") {
        server_port = atoi(argv[4]);
    } else if (string(argv[5]) == "-p") {
        server_port = atoi(argv[6]);
    } else if (string(argv[7]) == "-p") {
        server_port = atoi(argv[8]);
    } else if (string(argv[9]) == "-p") {
        server_port = atoi(argv[10]);
    } else {
        printf("lathos parametroi \n");
        printf("prepei na einai: -w numWorkers -b bufferSize -s serverIP  -p serverPort -i input_dir \n");
        return 0;
    }

    //    bool messages = true;

    printf("b = %d \n", bufferSize);
    printf("w = %d \n", w);
    printf("inputdir = %s \n", inputdir.c_str());
    printf("server_ip   = %s \n", server_ip.c_str());
    printf("server_port = %d \n", server_port);

    initializeCountries(inputdir);

    initializePipes(w); //arxikopoiisi

    initializeWorkers(bufferSize, server_ip, server_port);

    assignCountries();

    sendCountries();
    
    sendServerData(server_ip, server_port);

    static struct sigaction act; 
    act.sa_handler = catchinterrupt_aggregator; 
    sigfillset(&(act.sa_mask)); 
    sigaction(SIGINT, &act, NULL); 
    sigaction(SIGQUIT, &act, NULL);

    string line = "";

    while (true) {
        cout << "Aggregator (pid: " << getpid() << ") dose entoli: ";

      
        if (getline(cin, line)) { //dexetai entoles
            if (line == "exit" || line == "/exit") {
                cout << "exiting" << endl;
                cleanupWorkers();
                cleanupPipes();
                writeLog();
                break;
            }

            stringstream command(line);
            string entoli;

            string data[8] = {"", "", "", "", "", "", "", ""};

            command >> data[0] >> data[1] >> data[2] >> data[3] >> data[4] >> data[5] >> data[6] >> data[7];

            
            if (data[0] == "listCountries" || data[0] == "/listCountries") {
                listCountriesForAggregator();
            } else if (data[0] == "catin") {
                CountriesLinkedList * countries;
                countries = getCountriesAgg();
                CountriesNode* currentC;
                currentC = countries->head;
                int * fds = findPipeForCountry(currentC->rec->name);
                char * packet = new char[bufferSize];
                string tempS;
                while (currentC != NULL) {
                    cout << "Aggregator, com will use: " << fds[0] << " and " << fds[1] << endl;


                    fds = findPipeForCountry(currentC->rec->name);
                    tempS = line + " " + currentC->rec->name;
                    strcpy(packet, tempS.c_str());

                    write(fds[0], packet, bufferSize);

                    while (1) {
                        if (read(fds[1], packet, bufferSize) <= 0) { //pairnei tin apantisi
                            break;
                        }
                        if (strcmp(packet, "-") == 0) {
                            break;
                        } else {
                            cout << currentC->rec->name << " " << packet << endl;
                        }

                    }

                    currentC = currentC->next;
                }
                delete [] packet;
            } else if (data[0] == "/diseaseFrequency" || data[0] == "diseaseFrequency") {
                if (data[1] == "" || data[3] == "" || data[2] == "") {
                    cout << "error: you have to enter one virus name,two dates (and maybe a country-optional) " << endl;
                } else {
                    if (data[4] == "") {
                        // select

                        cout << "Broadcasting to all: " << line << endl;
                        broadcast(line);

                        collectDiseaseFrequencyWithSelect();

                    } else {
                        int * fds = findPipeForCountry(data[4]);

                        cout << "Aggregator, com will use: " << fds[0] << " and " << fds[1] << endl;

                        char * packet = new char[bufferSize];

                        strcpy(packet, line.c_str());

                        write(fds[0], packet, bufferSize);

                        while (1) {
                            if (read(fds[1], packet, bufferSize) <= 0) { //pairnei tin apantisi
                                break;
                            }
                            if (strcmp(packet, "-") == 0) {
                                break;
                            } else {
                                cout << packet << endl;
                            }

                        }

                        delete [] packet;
                    }
                }
            } else if (data[0] == "/topk-AgeRanges" || data[0] == "topk-AgeRanges") {
                if (data[1] == "" || data[2] == "" || (data[3] == "" && data[4] != "") || (data[3] != "" && data[4] == "")) {
                    cout << "error: you have to enter a number(top k), a country ,a disease and two dates" << endl;
                } else {
                    int * fds = findPipeForCountry(data[2]);

                    cout << "Aggregator, com will use: " << fds[0] << " and " << fds[1] << endl;

                    char * packet = new char[bufferSize];

                    strcpy(packet, line.c_str());

                    write(fds[0], packet, bufferSize);

                    while (1) {
                        if (read(fds[1], packet, bufferSize) <= 0) { //pairnei tin apantisi
                            break;
                        }
                        if (strcmp(packet, "-") == 0) {
                            break;
                        } else {
                            cout << packet << endl;
                        }

                    }

                    delete [] packet;


                }
            } else if (data[0] == "/numPatientAdmissions" || data[0] == "numPatientAdmissions") {
                if (data[1] == "" || data[3] == "" || data[2] == "") {
                    cout << "error: you have to enter one virus name,(maybe a country-optional) and two dates" << endl;
                } else {
                    if (data[4] == "") {
                        CountriesLinkedList * countries;
                        countries = getCountriesAgg();
                        CountriesNode* currentC;
                        currentC = countries->head;
                        int * fds = findPipeForCountry(currentC->rec->name);
                        char * packet = new char[bufferSize];
                        string tempS;
                        while (currentC != NULL) {
                            cout << "Aggregator, com will use: " << fds[0] << " and " << fds[1] << endl;


                            fds = findPipeForCountry(currentC->rec->name);
                            tempS = line + " " + currentC->rec->name;
                            strcpy(packet, tempS.c_str());

                            write(fds[0], packet, bufferSize);

                            while (1) {
                                if (read(fds[1], packet, bufferSize) <= 0) { //pairnei tin apantisi
                                    break;
                                }
                                if (strcmp(packet, "-") == 0) {
                                    break;
                                } else {
                                    cout << packet << endl;
                                }

                            }

                            currentC = currentC->next;
                        }
                        delete [] packet;
                    } else {
                        int * fds = findPipeForCountry(data[4]);

                        cout << "Aggregator, com will use: " << fds[0] << " and " << fds[1] << endl;

                        char * packet = new char[bufferSize];

                        strcpy(packet, line.c_str());

                        write(fds[0], packet, bufferSize);

                        while (1) {
                            if (read(fds[1], packet, bufferSize) <= 0) { //pairnei tin apantisi
                                break;
                            }
                            if (strcmp(packet, "-") == 0) {
                                break;
                            } else {
                                cout << packet << endl;
                            }

                        }

                        delete [] packet;
                    }

                }
            } else if (data[0] == "/searchPatientRecord" || data[0] == "searchPatientRecord") {
                if (data[1] == "") {
                    cout << "error: you have to enter one patient id" << endl;
                } else {
                    cout << "Broadcasting to all: " << line << endl;
                    broadcast(line);
                        
                    collectSearchRecordIdWithSelect();
                }
            } else if (data[0] == "/numPatientDischarges" || data[0] == "numPatientDischarges") {
                if (data[1] == "" || data[2] == "" || data[3] == "") {
                    cout << "error: you have to enter one virus name,(maybe a country-optional) and two dates" << endl;
                } else {
                    if (data[4] == "") {
                        CountriesLinkedList * countries;
                        countries = getCountriesAgg();
                        CountriesNode* currentC;
                        currentC = countries->head;
                        int * fds = findPipeForCountry(currentC->rec->name);
                        char * packet = new char[bufferSize];
                        string tempS;
                        while (currentC != NULL) {
                            cout << "Aggregator, com will use: " << fds[0] << " and " << fds[1] << endl;


                            fds = findPipeForCountry(currentC->rec->name);
                            tempS = line + " " + currentC->rec->name;
                            strcpy(packet, tempS.c_str());

                            write(fds[0], packet, bufferSize);

                            while (1) {
                                if (read(fds[1], packet, bufferSize) <= 0) { //pairnei tin apantisi
                                    break;
                                }
                                if (strcmp(packet, "-") == 0) {
                                    break;
                                } else {
                                    cout << packet << endl;
                                }

                            }

                            currentC = currentC->next;
                        }
                        delete [] packet;
                    } else {
                        int * fds = findPipeForCountry(data[4]);

                        cout << "Aggregator, com will use: " << fds[0] << " and " << fds[1] << endl;

                        char * packet = new char[bufferSize];

                        strcpy(packet, line.c_str());

                        write(fds[0], packet, bufferSize);

                        while (1) {
                            if (read(fds[1], packet, bufferSize) <= 0) { //pairnei tin apantisi
                                break;
                            }
                            if (strcmp(packet, "-") == 0) {
                                break;
                            } else {
                                cout << packet << endl;
                            }

                        }

                        delete [] packet;
                    }
                }
            } else {
                cout << "Invalid Command" << endl;
                cout << "You can use one of the following:" << endl;
                cout << "exit" << endl;
                cout << "insertPatientRecord" << endl;
                cout << "recordPatientExit" << endl;
                cout << "numCurrentPatients" << endl;
                cout << "globalDiseaseStats" << endl;
                cout << "diseaseFrequency" << endl;
                cout << "topk_Diseases" << endl;
                cout << "topk_Countries" << endl;

            }
        } else { //an vrei eof
            cleanupWorkers();
            cleanupPipes();
            writeLog();
            break;
        }

    }

    return 0;
}
