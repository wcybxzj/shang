main:moduleB.o moduleA.o
	    g++ moduleB.o moduleA.o -o main

moduleA.o:moduleA.c
	    gcc moduleA.c -c -Wall -g -o moduleA.o

moudleB.o:moduleB.cpp
	    g++ moduleB.cpp -c -Wall -g -o moduleB.o
clean:
	    rm *.o main -rf

