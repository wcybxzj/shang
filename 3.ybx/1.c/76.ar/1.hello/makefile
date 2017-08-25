all:hello.o hello1.o ar_work
hello.o:hello.h
hello1.o:hello1.h
ar_work:
	ar libtest.a hello.o hello1.o

clean:
	rm -rf *.o *.a
