CFLAGS+=-pthread -Wall
LDFLAGS+=-pthread

all:
	gcc 1.dup_dup2.c     -o 1.dup_dup2    $(CFLAGS)
	gcc 2.fork_fd.c      -o 2.fork_fd	  $(CFLAGS)
	gcc 3.pthread_fd.c   -o 3.pthread_fd  $(CFLAGS)

clean:
	$(RM) 1.dup_dup2 2.fork_fd 3.pthread_fd 
