CFLAGS+=-I.	

target:seqlist.o main.o
	gcc seqlist.o main.o $(CFLAGS) -o target

seqlist.o:seqlist.c
	gcc -o seqlist.c

main.o:main.c
	gcc -o main.c
