include ../Make.defines

PROGS = web02

all: ${PROGS}

${PROGS}: web02.o home_page.o write_get_cmd.o http.o
	${CC} ${CFLAGS} -o $@ $^  ${LIBS}

clean:
	rm -rf ${PROGS} ${CLEANFILES}


