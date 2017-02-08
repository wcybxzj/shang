CFLAGS+=-I ./include -pthread -lrt

LDFLAGS+=-pthread -lrt

all:3.log_test 4.mm_test 5.thread_primer 9.event_io_map-event_signal_map 10_1.event_base_new_with_config 10_2.event_get_supported_methods

3.log_test:3.log_test.o poll.o epoll.o log.o evutil.o

4.mm_test:4.mm_test.o evutil_rand.o event.o  evthread.o evthread_pthread.o evmap.o signal.o select.o poll.o epoll.o log.o evutil.o

5.thread_primer:5.thread_primer.o evutil_rand.o event.o evthread.o evthread_pthread.o evmap.o signal.o select.o poll.o epoll.o log.o evutil.o

9.event_io_map-event_signal_map: 9.event_io_map-event_signal_map.o evutil_rand.o event.o evthread.o evthread_pthread.o  evmap.o signal.o select.o poll.o epoll.o log.o evutil.o

10_1.event_base_new_with_config: 10_1.event_base_new_with_config.o evutil_rand.o event.o evthread.o evthread_pthread.o  evmap.o signal.o select.o poll.o epoll.o log.o evutil.o

10_2.event_get_supported_methods: 10_2.event_get_supported_methods.o evutil_rand.o event.o evthread.o evthread_pthread.o  evmap.o signal.o select.o poll.o epoll.o log.o evutil.o

clean:
	rm -rf *.o 3.log_test 4.mm_test 5.thread_primer  9.event_io_map-event_signal_map 10_1.event_base_new_with_config 10_2.event_get_supported_methods



