object = get.o sum.o val.o \
main.o
main:${object}
	gcc -o main ${object}
main.o:get.h sum.h
val.o:val.h
sum.o:val.h sum.h
get.o:get.h
.PHONY: clean cleanall
clean:
	rm -rf *.o
cleanall:clean
	rm -rf main
