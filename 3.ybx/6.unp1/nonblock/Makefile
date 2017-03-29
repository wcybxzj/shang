include ../Make.defines

PROGS = web tcpcli02

all: ${PROGS}

tcpcli02: tcpcli02.o strclinonb.o
	${CC} ${CFLAGS} -o $@ $^  ${LIBS}

web: web.o home_page.o start_connect.o write_get_cmd.o
	${CC} ${CFLAGS} -o $@ $^  ${LIBS}

clean:
	rm -rf ${PROGS} ${CLEANFILES}


