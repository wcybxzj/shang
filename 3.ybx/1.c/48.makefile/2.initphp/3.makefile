include 3.test.vm
main:${object}
	gcc -o main ${object}
main.o:get.h sum.h
val.o:val.h
sum.o:val.h sum.h
get.o:get.h
clean:
	rm -rf *.o main
