CFLAGS+=-I../include
CFLAGS+=-pthread
DFLAGS+=-pthread

all:server

server:mytbf.o medialib.o thr_channle.o server.o
	gcc $^ -o $@ $(CFLAGS) $(DFLAGS)

clean:
	rm -rf *.o server
