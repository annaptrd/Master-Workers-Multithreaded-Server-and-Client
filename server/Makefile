objects = main.cpp main_server_threads.cpp methods_server.cpp BST.cpp BucketNode.cpp CountriesList.cpp Data.cpp DatesList.cpp HashTable.cpp Record.cpp dateToInt.cpp hashFunction.cpp inbetweenDates.cpp queue.cpp RecordsList.cpp methods.cpp

whoServer: $(objects)
	g++ -std=c++11 -g $(objects) -o whoServer -lpthread

main.o: BucketNode.h methods_server.h CountriesList.h inbetweenDates.h BST.h queue.h Data.h
methods.o: methods.h HashTable.h dateToInt.h ChildParameters.h CountriesList.h RecordsList.h
methods_server.o: methods.h methods_server.h CountriesList.h ChildParameters.h
main_server_threads.o: dateToInt.h BucketNode.h methods.h inbetweenDates.h BST.h CountriesList.h DatesList.h ChildParameters.h methods_server.h
CountriesList.o: CountriesList.h
DatesList.o: DatesList.h dateToInt.h
RecordsList.o: RecordsList.h
inbetweenDates.o: dateToInt.h
hashFunction.o: hashFunction.h
dateToInt.o: Record.h
Record.o: Record.h dateToInt.h
HashTable.o: HashTable.h hashFunction.h
Data.o: Data.h CountriesList.h
BucketNode.o: BucketNode.h
BST.o: BST.h dateToInt.h inbetweenDates.h RecordsList.h
queue.o: queue.h

	clean :
			rm whoServer $(objects)
