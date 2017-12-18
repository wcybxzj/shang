PROGS = 1.test_zmalloc 2.test_sds 3.test_adlist 4.test_dict
all:$(PROGS)

1.test_zmalloc:1.test_zmalloc.o sds.o dict.o  adlist.o zmalloc.o
	$(CC) $(CFLAGS) -o 1.test_zmalloc 1.test_zmalloc.o sds.o  dict.o  adlist.o  zmalloc.o

2.test_sds:2.test_sds.o sds.o dict.o  adlist.o zmalloc.o
	$(CC) $(CFLAGS) -o 2.test_sds 2.test_sds.o sds.o  dict.o  adlist.o  zmalloc.o

3.test_adlist: 3.test_adlist.o sds.o dict.o  adlist.o zmalloc.o
	$(CC) $(CFLAGS) -o 3.test_adlist 3.test_adlist.o sds.o  dict.o  adlist.o  zmalloc.o

4.test_dict: 4.test_dict.o sds.o dict.o  adlist.o zmalloc.o
	$(CC) $(CFLAGS) -o 4.test_dict 4.test_dict.o sds.o  dict.o  adlist.o  zmalloc.o

sds.o: sds.c sds.h zmalloc.h
dict.o: dict.c dict.h zmalloc.h redisassert.h fmacros.h

clean:
	rm -f $(PROGS) *.o
