#variable CC will be the compiler to use.
CC = g++
#CFLAGS will be the options I'll pass to the compiler.
CFLAGS = -c -Wall

all: clean main

main: main.o createSizes.o precsfr.o topology.o csfr.o simulator.o

	$(CC) main.o createSizes.o precsfr.o csfr.o simulator.o topology.o -o rlmanager -lrt
	
main.o: main.cpp

	$(CC) $(CFLAGS) main.cpp
		
createSizes.o: createSizes.cpp

	$(CC) $(CFLAGS) createSizes.cpp

precsfr.o: precsfr.cpp

	$(CC) $(CFLAGS) precsfr.cpp
	
csfr.o: csfr.cpp

	$(CC) $(CFLAGS) csfr.cpp
	
simulator.o: simulator.cpp
	
	$(CC) $(CFLAGS) simulator.cpp
	
topology.o: topology.cpp
	
	$(CC) $(CFLAGS) topology.cpp	
	
clean:

	rm -rf *o *txt rlmanager
	clear
