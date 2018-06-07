PROGS=1.binary_search
OBJS=1.binary_search.o
CFLAGS+=-c -Wall -g

all:$(PROGS)

1.binary_search:$(OBJS)
	$(CC) $^ -o $@

%.o:%.c
	$(CC) $^ $(CFLAGS) -o $@

clean:
	rm -f $(PROGS) *.o a.out

