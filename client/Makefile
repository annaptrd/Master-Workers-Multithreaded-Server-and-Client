objects = main.cpp main_threads.cpp CommandsList.cpp methods_client.cpp

whoClient​: $(objects)
	g++ -std=c++11 -g $(objects) -o whoClient -lpthread

main.o: CommandsList.h methods_client.h
main_threads.o: methods_client.h CommandsList.h ChildParameters.h
methods_client.o: CommandsList.h methods_client.h methods.h ChildParameters.h
CommandsList.o: CommandsList.h

	clean :
			rm whoClient​ $(objects)
