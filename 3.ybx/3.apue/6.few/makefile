WARN=-Wall -W
FINAL_CFLAGS= $(STD) $(WARN) -g -g3 -gdwarf-2 -lm -lpthread

PROGS=2.waitpid
all:$(PROGS)

2.waitpid:2.waitpid.o
	$(CC) $(FINAL_CFLAGS) -o 2.waitpid 2.waitpid.o

%.o: %.c
	$(CC) $(FINAL_CFLAGS) -c $<


clean:
	rm -f $(PROGS) *.o a.out
