include ../Make.defines

PROGS = client

all: ${PROGS}

client:client.o
	${CC} ${CFLAGS} -o $@ $^  ${LIBS}

clean:
	rm -rf ${PROGS} ${CLEANFILES}


