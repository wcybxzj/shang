include ../Make.defines

PROGS = web

all: ${PROGS}

web: web.o home_page.o start_connect.o write_get_cmd.o
	${CC} ${CFLAGS} -o $@ web.o home_page.o start_connect.o \
		write_get_cmd.o ${LIBS}

clean:
	rm -rf ${PROGS} ${CLEANFILES}


