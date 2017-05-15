object = get.o sum.o val.o \
main.o
main:$(object)
	@echo it is compile main.c now!..............
	$(CC) -o $@ $(object)
	$(MAKE) -f 6.makefile clean
main.o:get.h sum.h
val.o:val.h
sum.o:val.h sum.h
get.o:get.h
.PHONY: clean cleanall
clean:
	-rm -rf *.o
cleanall:clean
	-rm -rf main
