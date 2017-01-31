CFLAGS+=-I ./include -pthread

LDFLAGS+=-pthread
all:3.log_test 4.mm_test 5.thread_primer

3.log_test:3.log_test.o log.o evutil.o

4.mm_test:4.mm_test.o evutil_rand.o event.o  evthread.o evthread_pthread.o signal.o log.o evutil.o

5.thread_primer:5.thread_primer.o evutil_rand.o event.o evthread.o evthread_pthread.o signal.o log.o evutil.o

clean:
	rm -rf *.o 3.log_test 4.mm_test 5.thread_primer



