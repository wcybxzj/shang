OPTIMIZATION?=-O0

STD=-std=c99 -pedantic -DREDIS_STATIC=''
WARN=-Wall -W
OPT=$(OPTIMIZATION)

#FINAL_CFLAGS=$(STD) $(WARN) $(OPT) $(DEBUG) $(CFLAGS)   $(REDIS_CFLAGS) -I../deps/geohash-int
FINAL_LDFLAGS=$(LDFLAGS) $(REDIS_LDFLAGS) $(DEBUG)
FINAL_LIBS=-lm
DEBUG=-g

FINAL_LDFLAGS+= -rdynamic
FINAL_LIBS+= -pthread

CCCOLOR="\033[34m"
LINKCOLOR="\033[34;1m"
SRCCOLOR="\033[33m"
BINCOLOR="\033[37;1m"
MAKECOLOR="\033[32;1m"
ENDCOLOR="\033[0m"

ifndef V
QUIET_CC = @printf '    %b %b\n' $(CCCOLOR)CC$(ENDCOLOR) $(SRCCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_LINK = @printf '    %b %b\n' $(LINKCOLOR)LINK$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_INSTALL = @printf '    %b %b\n' $(LINKCOLOR)INSTALL$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
endif

# Default settings
STD=-std=c99 -pedantic -DREDIS_STATIC=''
WARN=-Wall -W

FINAL_CFLAGS= $(STD) $(WARN) -g -g3 -gdwarf-2 -lm -lpthread
FINAL_CFLAGS+= -I../deps/hiredis -I../deps/linenoise -I../deps/lua/src

REDIS_CC=$(CC) $(FINAL_CFLAGS)
REDIS_LD=$(QUIET_LINK)$(CC) $(FINAL_LDFLAGS)

REDIS_SERVER_OBJ=  14.test_t_zset.o ybx_util.o

REDIS_SERVER_OBJ+= ae.o anet.o dict.o server.o sds.o adlist.o ziplist.o  release.o zmalloc.o networking.o util.o sha1.o endianconv.o \
					object.o db.o  rdb.o replication.o t_zset.o t_hash.o  config.o aof.o t_string.o t_list.o t_set.o pubsub.o multi.o debug.o cluster.o \
					intset.o syncio.o quicklist.o lzf_c.o lzf_d.o scripting.o bio.o rio.o crc64.o notify.o setproctitle.o blocked.o latency.o redis-check-rdb.o

REDIS_CLI_OBJ = anet.o adlist.o zmalloc.o anet.o ae.o crc64.o

PROGS = 1.test_zmalloc 2.test_sds 3.test_adlist 4.test_dict 5.test_ziplist 6.test_skiplist 7.test_intset\
		8.test_object 9.test_quicklist 10.test_t_list 11.test_t_set 12.test_t_set 13.test_t_hash 14.test_t_zset_main \
		15.test_db 18.rdb 20.ae 21.redis-cli

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

8.test_object: 8.test_object.o $(REDIS_SERVER_OBJ)
	$(CC) $(FINAL_CFLAGS) -o 8.test_object 8.test_object.o $(REDIS_SERVER_OBJ)

9.test_quicklist: 9.test_quicklist.o $(REDIS_SERVER_OBJ)
	$(CC) $(FINAL_CFLAGS) -o 9.test_quicklist 9.test_quicklist.o $(REDIS_SERVER_OBJ)

10.test_t_list: 10.test_t_list.o $(REDIS_SERVER_OBJ)
	$(CC) $(FINAL_CFLAGS) -o 10.test_t_list 10.test_t_list.o $(REDIS_SERVER_OBJ)

11.test_t_set: 11.test_t_set.o $(REDIS_SERVER_OBJ)
	$(CC) $(FINAL_CFLAGS) -o 11.test_t_set 11.test_t_set.o $(REDIS_SERVER_OBJ)

12.test_t_set: 12.test_t_set.o $(REDIS_SERVER_OBJ)
	$(CC) $(FINAL_CFLAGS) -o 12.test_t_set 12.test_t_set.o $(REDIS_SERVER_OBJ)

13.test_t_hash: 13.test_t_hash.o $(REDIS_SERVER_OBJ)
	$(CC) $(FINAL_CFLAGS) -o 13.test_t_hash 13.test_t_hash.o $(REDIS_SERVER_OBJ)

14.test_t_zset_main: 14.test_t_zset_main.o $(REDIS_SERVER_OBJ)
	$(CC) $(FINAL_CFLAGS) -o 14.test_t_zset_main 14.test_t_zset_main.o $(REDIS_SERVER_OBJ)

15.test_db: 15.test_db.o $(REDIS_SERVER_OBJ)
	$(CC) $(FINAL_CFLAGS) -o 15.test_db 15.test_db.o $(REDIS_SERVER_OBJ)

18.rdb: 18.rdb.o $(REDIS_SERVER_OBJ)
	$(CC) $(FINAL_CFLAGS) -o 18.rdb 18.rdb.o $(REDIS_SERVER_OBJ)

20.ae: 20.ae.o $(REDIS_SERVER_OBJ)
	$(CC) $(FINAL_CFLAGS) -o 20.ae 20.ae.o $(REDIS_SERVER_OBJ)

21.redis-cli:21.redis-cli.o $(REDIS_CLI_OBJ)
	$(REDIS_LD) -o $@ $^ ../deps/hiredis/libhiredis.a $(FINAL_LIBS)


%.o: %.c
	$(REDIS_CC) -c $<

include Makefile.dep

clean:
	rm -f $(PROGS) *.o

distclean: clean
	(cd ../deps && $(MAKE) distclean)
	(rm -f .make-*)

