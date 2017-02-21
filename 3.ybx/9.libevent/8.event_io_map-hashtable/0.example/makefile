CFLAGS+=-I ./include -pthread -DWIN32 -Wall -lrt
LDFLAGS+=-pthread -lrt

all: 8.event_io_map-hashtable

8.event_io_map-hashtable:8.event_io_map-hashtable.o evmap.o evutil_rand.o event.o evthread.o evthread_pthread.o signal.o log.o evutil.o select.o poll.o epoll.o 

clean:
	rm -rf *.o  8.event_io_map-hashtable
