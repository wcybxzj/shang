main:get.o sum.o val.o main.o
	gcc -o main get.o sum.o val.o main.o
main.o:main.c
	gcc -c main.c
val.o:val.h val.c
	gcc -c val.c
sum.o:val.h sum.h sum.c
	gcc -c sum.c
get.o:get.h get.c
	gcc -c get.c
clean:
	rm -rf *.o
