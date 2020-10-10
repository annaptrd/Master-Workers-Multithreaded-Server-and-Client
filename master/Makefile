objects = main.cpp main_worker.cpp methods_aggregator.cpp BST.cpp BucketNode.cpp CountriesList.cpp Data.cpp DatesList.cpp HashTable.cpp Record.cpp dateToInt.cpp hashFunction.cpp inbetweenDates.cpp RecordsList.cpp methods.cpp

master: $(objects)
	g++ -std=c++11 -g $(objects) -o master -lpthread

main.o: BucketNode.h methods_aggregator.h CountriesList.h inbetweenDates.h BST.h
methods.o: methods.h HashTable.h dateToInt.h ChildParameters.h CountriesList.h RecordsList.h
methods_aggregator.o: methods.h methods_aggregator.h CountriesList.h ChildParameters.h
main_worker.o: dateToInt.h BucketNode.h methods.h inbetweenDates.h BST.h CountriesList.h DatesList.h ChildParameters.h
CountriesList.o: CountriesList.h
DatesList.o: DatesList.h dateToInt.h
RecordsList.o: RecordsList.h
inbetweenDates.o: dateToInt.h
hashFunction.o: hashFunction.h
dateToInt.o: Record.h
Record.o: Record.h dateToInt.h
HashTable.o: HashTable.h hashFunction.h
Data.o: Data.h
BucketNode.o: BucketNode.h
BST.o: BST.h dateToInt.h inbetweenDates.h RecordsList.h

	clean :
			rm master $(objects)
