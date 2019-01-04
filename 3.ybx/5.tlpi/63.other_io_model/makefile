WARN=-Wall -W

FINAL_CFLAGS= $(STD) $(WARN) -g -g3 -gdwarf-2 -lm -lpthread

PROGS=1.epoll_input 63.2.1.t_select 63.4.3.epoll_oneshot 63.4.6.epoll_et1 63.4.6.epoll_et2 63.4.6.epoll_et3 63.4.6.epoll_et4 63.4.6.epoll_lt 63.5.1.pselect 63.5.2.self_pipe 63.5.select

all:$(PROGS)

1.epoll_input:         1.epoll_input.o
	$(CC) $^ $(FINAL_CFLAGS) -o $@

63.2.1.t_select:       63.2.1.t_select.o
	$(CC) $^ $(FINAL_CFLAGS) -o $@

63.4.3.epoll_oneshot:  63.4.3.epoll_oneshot.o
	$(CC) $^ $(FINAL_CFLAGS) -o $@

63.4.6.epoll_et1:       63.4.6.epoll_et1.o
	$(CC) $^ $(FINAL_CFLAGS) -o $@

63.4.6.epoll_et2:       63.4.6.epoll_et2.o
	$(CC) $^ $(FINAL_CFLAGS) -o $@

63.4.6.epoll_et3:       63.4.6.epoll_et3.o
	$(CC) $^ $(FINAL_CFLAGS) -o $@

63.4.6.epoll_et4:       63.4.6.epoll_et4.o
	$(CC) $^ $(FINAL_CFLAGS) -o $@

63.4.6.epoll_lt:       63.4.6.epoll_lt.o
	$(CC) $^ $(FINAL_CFLAGS) -o $@

63.5.1.pselect:        63.5.1.pselect.o
	$(CC) $^ $(FINAL_CFLAGS) -o $@

63.5.2.self_pipe:      63.5.2.self_pipe.o
	$(CC) $^ $(FINAL_CFLAGS) -o $@

63.5.select:           63.5.select.o
	$(CC) $^ $(FINAL_CFLAGS) -o $@

clean:
	rm -f $(PROGS) *.o a.out 123 456
