$(CC)=gcc

OBJ=lib.o

PROGS=1.server 2.client

all:$(PROGS) $(OBJ)

1.server:1.server.o $(OBJ)
	$(CC) -o 1.server 1.server.o $(OBJ)

2.client:2.client.o $(OBJ)
	$(CC) -o 2.client 2.client.o $(OBJ)

%.o: %.c
	$(CC) -c $<

lib.o:lib.c header.h

clean:
	rm -rf *.o $(PROGS)

