PROGS= 1.int_to_string 2.fibonacci
OBJS= 1.int_to_string.o 2.fibonacci.o
CFLAGS+=-c -Wall -g

all:$(PROGS)

1.int_to_string:1.int_to_string.o
	$(CC) $^ -o $@

2.fibonacci:2.fibonacci.o
	$(CC) $^ -o $@

%.o:%.c
	$(CC) $^ $(CFLAGS) -o $@

clean:
	rm -f $(PROGS) *.o a.out

