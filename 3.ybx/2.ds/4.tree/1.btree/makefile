CFLAGS=-I./include -L./lib -Wall -g -lqueue -lllist
all:main

main:main.o
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	$(RM) *.o main
