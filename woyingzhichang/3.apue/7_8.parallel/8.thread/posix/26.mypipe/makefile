CFLAGS+=-pthread
LDFLAGS+=-pthread
all:main

main:main.o queue.o mypipe.o
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)  -Wall

clean:
	$(RM) main *.o


