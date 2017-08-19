#touch program.c foo.c utils.c defs.h

objects = program.o foo.o utils.o
program : $(objects)
	cc -o program $(objects)

$(objects) : defs.h

clean:
	rm -rf $(objects) program
