CFLAGS= -g
PROGS = 1.test_zmalloc 2.test_sds 3.test_adlist 4.test_dict 5.test_ziplist
all:$(PROGS)

1.test_zmalloc:1.test_zmalloc.o sds.o dict.o  adlist.o zmalloc.o
	$(CC) $(CFLAGS) -o 1.test_zmalloc 1.test_zmalloc.o sds.o  dict.o  adlist.o  zmalloc.o

2.test_sds:2.test_sds.o sds.o dict.o  adlist.o zmalloc.o
	$(CC) $(CFLAGS) -o 2.test_sds 2.test_sds.o sds.o  dict.o  adlist.o  zmalloc.o

3.test_adlist: 3.test_adlist.o sds.o dict.o  adlist.o zmalloc.o
	$(CC) $(CFLAGS) -o 3.test_adlist 3.test_adlist.o sds.o  dict.o  adlist.o  zmalloc.o

4.test_dict: 4.test_dict.o sds.o dict.o  adlist.o zmalloc.o
	$(CC) $(CFLAGS) -o 4.test_dict 4.test_dict.o sds.o  dict.o  adlist.o  zmalloc.o

5.test_ziplist: 5.test_ziplist.o sds.o dict.o  adlist.o ziplist.o zmalloc.o util.o sha1.o endianconv.o
	$(CC) $(CFLAGS) -o 5.test_ziplist 5.test_ziplist.o sds.o  dict.o  adlist.o ziplist.o zmalloc.o util.o sha1.o endianconv.o

sds.o: sds.c sds.h zmalloc.h
dict.o: dict.c dict.h zmalloc.h redisassert.h fmacros.h
ziplist.o:ziplist.c ziplist.h zmalloc.h
sha1.o:sha1.c sha1.h config.h
util.o: util.c fmacros.h util.h sds.h
ziplist.o: ziplist.c zmalloc.h util.h sds.h ziplist.h endianconv.h \
config.h redisassert.h

clean:
	rm -f $(PROGS) *.o
