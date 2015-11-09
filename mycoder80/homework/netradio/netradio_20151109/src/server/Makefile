CFLAGS+=-I../include
CFLAGS+=-pthread
LDFLAGS+=-pthread

all:server

server:mytbf.o medialib.o thr_channle.o thr_list.o server.o
	gcc $^ -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf *.o server
