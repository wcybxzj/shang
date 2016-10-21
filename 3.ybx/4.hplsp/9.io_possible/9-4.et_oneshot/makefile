CFLAGS+=-pthread
LDFLAGS+=-pthread

all:
	gcc 1.no_oneshot.c       -o 1.no_oneshot $(CFLAGS)
	gcc 2.oneshot.c          -o 2.oneshot	 $(CFLAGS)
	gcc 3.client.c           -o 3.client	 $(CFLAGS)
	gcc 4.client_sleep.c     -o 4.client_sleep $(CFLAGS)

clean:
	rm  -f 1.no_oneshot 2.oneshot 3.client 4.client_sleep
