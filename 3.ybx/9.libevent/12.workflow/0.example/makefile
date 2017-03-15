CFLAGS+=-I ./include -I ./compat -pthread -lrt

LDFLAGS+=-pthread -lrt
OBJS=listener.o bufferevent_sock.o bufferevent_ratelim.o buffer.o bufferevent.o evutil_rand.o event.o evthread.o evthread_pthread.o  evmap.o signal.o select.o poll.o epoll.o log.o evutil.o strlcpy.o

all:3.log_test 4.mm_test 5.thread_primer 9.event_io_map-event_signal_map 10_1.event_base_new_with_config 10_2.event_get_supported_methods 12_1.workflow 14_1.signal_problem 14_2.signal_ok 15_1.regress_minheap 15_2.thread_add_signal_event 15_3.thread_add_timeout_event 16_1.timeout_event 18_1.common_timeout 21.evconnlistener

3.log_test:3.log_test.o $(OBJS)

4.mm_test:4.mm_test.o  $(OBJS)

5.thread_primer:5.thread_primer.o  $(OBJS)

9.event_io_map-event_signal_map: 9.event_io_map-event_signal_map.o  $(OBJS)

10_1.event_base_new_with_config: 10_1.event_base_new_with_config.o  $(OBJS)

10_2.event_get_supported_methods: 10_2.event_get_supported_methods.o  $(OBJS)

12_1.workflow: 12_1.workflow.o  $(OBJS)

14_1.signal_problem: 14_1.signal_problem.o  $(OBJS)

14_2.signal_ok: 14_2.signal_ok.o  $(OBJS)

15_1.regress_minheap: 15_1.regress_minheap.o  $(OBJS)

15_2.thread_add_signal_event: 15_2.thread_add_signal_event.o  $(OBJS)

15_3.thread_add_timeout_event: 15_3.thread_add_timeout_event.o  $(OBJS)

16_1.timeout_event:16_1.timeout_event.o  $(OBJS)

18_1.common_timeout:18_1.common_timeout.o  $(OBJS)

21.evconnlistener: 21.evconnlistener.o $(OBJS)

clean:
	rm -rf *.o 3.log_test 4.mm_test 5.thread_primer 9.event_io_map-event_signal_map 10_1.event_base_new_with_config 10_2.event_get_supported_methods 12_1.workflow 14_1.signal_problem 14_2.signal_ok 15_1.regress_minheap 15_2.thread_add_signal_event 15_3.thread_add_timeout_event 16_1.timeout_event 18_1.common_timeout
