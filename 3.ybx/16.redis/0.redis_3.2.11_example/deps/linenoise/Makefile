STD=
WARN= -Wall
OPT= -O0

R_CFLAGS= $(STD) $(WARN) $(OPT) $(DEBUG) $(CFLAGS) -g3 -gdwarf-2
R_LDFLAGS= $(LDFLAGS)
DEBUG= -g

R_CC=$(CC) $(R_CFLAGS)
R_LD=$(CC) $(R_LDFLAGS)

linenoise.o: linenoise.h linenoise.c

linenoise_example: linenoise.o
	$(R_LD) -o $@ $^

.c.o:
	$(R_CC) -c $<

clean:
	rm -f linenoise_example *.o
