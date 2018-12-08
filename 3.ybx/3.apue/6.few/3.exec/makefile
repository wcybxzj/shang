$(CC)=gcc

PROGS= 3.exec 4.exec 5.exec 6.FD_CLOEXEC 7.exec

all:$(PROGS)

3.exec:3.exec.o
	$(CC) -o 3.exec 3.exec.o

4.exec:4.exec.o
	$(CC) -o 4.exec 4.exec.o

5.exec:5.exec.o
	$(CC) -o 5.exec 5.exec.o

6.FD_CLOEXEC:6.FD_CLOEXEC.o
	$(CC) -o 6.FD_CLOEXEC 6.FD_CLOEXEC.o

7.exec:7.exec.o
	$(CC) -o 7.exec 7.exec.o

%.o: %.c
	$(CC) -c $<


clean:
	rm -rf *.o $(PROGS)

