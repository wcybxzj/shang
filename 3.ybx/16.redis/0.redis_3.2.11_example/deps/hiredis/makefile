AE_DIR=../../src
EXAMPLES=1.hiredis-reply 2.example-ae 3.example-ae-sub

OBJ=net.o hiredis.o sds.o async.o
LIBNAME=libhiredis

CC:=$(shell sh -c 'type $(CC) >/dev/null 2>/dev/null && echo $(CC) || echo gcc')
OPTIMIZATION?=-O0
WARNINGS=-Wall -W -Wstrict-prototypes -Wwrite-strings
DEBUG?= -g -ggdb -g3 -gdwarf-2
REAL_CFLAGS=$(OPTIMIZATION) -fPIC $(CFLAGS) $(WARNINGS) $(DEBUG) $(ARCH)
REAL_LDFLAGS=$(LDFLAGS) $(ARCH)

STLIBSUFFIX=a
STLIBNAME=$(LIBNAME).$(STLIBSUFFIX)
 STLIB_MAKE_CMD=ar rcs $(STLIBNAME)

$(STLIBNAME): $(OBJ)
	$(STLIB_MAKE_CMD) $(OBJ)

static: $(STLIBNAME)

# Deps (use make dep to generate this)
net.o: net.c fmacros.h net.h hiredis.h
async.o: async.c async.h hiredis.h sds.h dict.c dict.h
hiredis.o: hiredis.c fmacros.h hiredis.h net.h sds.h
sds.o: sds.c sds.h
test.o: test.c hiredis.h

1.hiredis-reply:1.hiredis-reply.c $(STLIBNAME)
	$(CC) -o $@ $(REAL_CFLAGS) $(REAL_LDFLAGS) -I. $< $(STLIBNAME)

2.example-ae:2.example-ae.c  adapters/ae.h $(STLIBNAME)
	$(CC) -o $@ $(REAL_CFLAGS) $(REAL_LDFLAGS) -I. -I$(AE_DIR) $< $(AE_DIR)/ae.o $(AE_DIR)/zmalloc.o -pthread $(STLIBNAME)

3.example-ae-sub:3.example-ae-sub.c  adapters/ae.h $(STLIBNAME)
	$(CC) -o $@ $(REAL_CFLAGS) $(REAL_LDFLAGS) -I. -I$(AE_DIR) $< $(AE_DIR)/ae.o $(AE_DIR)/zmalloc.o -pthread $(STLIBNAME)

4.example-sub: 4.example-sub.c $(STLIBNAME)
	$(CC) -o $@ $(REAL_CFLAGS) $(REAL_LDFLAGS) -I. $< $(STLIBNAME)

examples: $(EXAMPLES)

.c.o:
	$(CC) -std=c99 -pedantic -c $(REAL_CFLAGS) $<

clean:
	rm -rf $(DYLIBNAME) $(STLIBNAME) $(TESTS) examples/hiredis-example* *.o *.gcda *.gcno *.gcov
