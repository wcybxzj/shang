PROGS = 1.bubble
CFLAGS=-g -g3 -gdwarf-2


all:$(PROGS)

1.bubble:1.bubble.o func.o
	$(CC) $(CFLAGS) -o 1.bubble 1.bubble.o func.o

clean:
	rm -f a.out *.o $(PROGS)
