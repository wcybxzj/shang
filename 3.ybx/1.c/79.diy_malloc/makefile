CC=cc -g3 -DDEBUG
PROGS = 1.toy_malloc 2.test_my_malloc
all:$(PROGS)

1.toy_malloc:1.toy_malloc.o
	$(CC) -o 1.toy_malloc 1.toy_malloc.o
2.test_my_malloc:2.test_my_malloc.o my_malloc.o
	$(CC) -o 2.test_my_malloc 2.test_my_malloc.o my_malloc.o
clean:
	rm -f $(PROGS) *.o

