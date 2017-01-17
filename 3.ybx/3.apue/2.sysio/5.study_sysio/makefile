CFLAGS+=-pthread -Wall
LDFLAGS+=-pthread

all:
	gcc 1.fork.c      -o 1.fork	  $(CFLAGS)
	gcc 2.pthread.c   -o 2.pthread  $(CFLAGS)

clean:
	$(RM) 1.fork 2.pthread
