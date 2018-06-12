PROGS= 0.divisor 1.int_to_string 2.fibonacci
CFLAGS+=-c -Wall -g

all:$(PROGS)

0.divisor:0.divisor.o
	$(CC) $^ -o $@

1.int_to_string:1.int_to_string.o
	$(CC) $^ -o $@

2.fibonacci:2.fibonacci.o
	$(CC) $^ -o $@

%.o:%.c
	$(CC) $^ $(CFLAGS) -o $@

clean:
	rm -f $(PROGS) *.o a.out

