object = get.o sum.o val.o \
main.o
main:test ${object}
	gcc -o $@ ${object}
test:${object}
	gcc -o $@ ${object}
main.o:get.h sum.h
val.o:val.h
sum.o:val.h sum.h
get.o:get.h
.PHONY: clean cleanall
clean:
	rm -rf *.o
cleanall:clean
	rm -rf main
