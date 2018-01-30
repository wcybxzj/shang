$(CC)=gcc

OBJ=lib.o

PROGS=1.server

all:$(PROGS) $(OBJ)

1.server:1.server.o $(OBJ)
	$(CC) -o 1.server 1.server.o $(OBJ)

%.o: %.c
	$(CC) -c $<

lib.o:lib.c header.h

clean:
	rm -rf *.o $(PROGS)

