include ../Make.defines

PROGS = web02 web03

all: ${PROGS}

web02: web02.o home_page.o write_get_cmd.o http.o
	${CC} ${CFLAGS} -o $@ $^  ${LIBS}

web03: web03.o home_page.o write_get_cmd.o http.o
	${CC} ${CFLAGS} -o $@ $^  ${LIBS}

clean:
	rm -f ${PROGS} ${CLEANFILES} *.jpg *.txt


