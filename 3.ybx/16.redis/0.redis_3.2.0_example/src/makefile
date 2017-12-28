
# Default settings
STD=-std=c99 -pedantic -DREDIS_STATIC=''
WARN=-Wall -W

FINAL_CFLAGS= $(STD) $(WARN) -g -g3 -gdwarf-2 -lm
FINAL_CFLAGS+= -I../deps/hiredis -I../deps/linenoise -I../deps/lua/src

REDIS_CC=$(CC) $(FINAL_CFLAGS)

REDIS_SERVER_OBJ=t_zset.o sds.o dict.o  adlist.o ziplist.o zmalloc.o util.o sha1.o endianconv.o object.o debug.o intset.o

PROGS = 1.test_zmalloc 2.test_sds 3.test_adlist 4.test_dict 5.test_ziplist 6.test_skiplist 7.test_intset
all:$(PROGS)

1.test_zmalloc:1.test_zmalloc.o sds.o dict.o  adlist.o zmalloc.o
	$(CC) $(FINAL_CFLAGS) -o 1.test_zmalloc 1.test_zmalloc.o sds.o  dict.o  adlist.o  zmalloc.o

2.test_sds:2.test_sds.o sds.o dict.o  adlist.o zmalloc.o
	$(CC) $(FINAL_CFLAGS) -o 2.test_sds 2.test_sds.o sds.o  dict.o  adlist.o  zmalloc.o

3.test_adlist: 3.test_adlist.o sds.o dict.o  adlist.o zmalloc.o
	$(CC) $(FINAL_CFLAGS) -o 3.test_adlist 3.test_adlist.o sds.o  dict.o  adlist.o  zmalloc.o

4.test_dict: 4.test_dict.o sds.o dict.o  adlist.o zmalloc.o
	$(CC) $(FINAL_CFLAGS) -o 4.test_dict 4.test_dict.o sds.o  dict.o  adlist.o  zmalloc.o

5.test_ziplist: 5.test_ziplist.o sds.o dict.o  adlist.o ziplist.o zmalloc.o util.o sha1.o endianconv.o
	$(CC) $(FINAL_CFLAGS) -o 5.test_ziplist 5.test_ziplist.o sds.o  dict.o  adlist.o ziplist.o zmalloc.o util.o sha1.o endianconv.o

6.test_skiplist: 6.test_skiplist.o $(REDIS_SERVER_OBJ)
	$(CC) $(FINAL_CFLAGS) -o 6.test_skiplist 6.test_skiplist.o $(REDIS_SERVER_OBJ)

7.test_intset: 7.test_intset.o $(REDIS_SERVER_OBJ)
	$(CC) $(FINAL_CFLAGS) -o 7.test_intset 7.test_intset.o $(REDIS_SERVER_OBJ)
%.o: %.c
	$(REDIS_CC) -c $<

include Makefile.dep

clean:
	rm -f $(PROGS) *.o
