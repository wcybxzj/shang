#include "server.h"
#include "cluster.h"
#include "slowlog.h"
#include "bio.h"
#include "latency.h"

#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <locale.h>
#include <sys/socket.h>

// 共享对象结构，初始化在createSharedObjects()函数
struct sharedObjectsStruct shared;
// 全局double类型常量
double R_Zero, R_PosInf, R_NegInf, R_Nan;

/*================================= Globals ================================= */

/* Global vars */
// 服务器状态结构
struct redisServer server; /* server global state */

/* Our command table.
 *
 * Every entry is composed of the following fields:
 *
 * name: a string representing the command name.
 * function: pointer to the C function implementing the command.
 * arity: number of arguments, it is possible to use -N to say >= N
 * sflags: command flags as string. See below for a table of flags.
 * flags: flags as bitmask. Computed by Redis using the 'sflags' field.
 * get_keys_proc: an optional function to get key arguments from a command.
 *                This is only used when the following three fields are not
 *                enough to specify what arguments are keys.
 * first_key_index: first argument that is a key
 * last_key_index: last argument that is a key
 * key_step: step to get all the keys from first to last argument. For instance
 *           in MSET the step is two since arguments are key,val,key,val,...
 * microseconds: microseconds of total execution time for this command.
 * calls: total number of calls of this command.
 *
 * The flags, microseconds and calls fields are computed by Redis and should
 * always be set to zero.
 *
 * Command flags are expressed using strings where every character represents
 * a flag. Later the populateCommandTable() function will take care of
 * populating the real 'flags' field using this characters.
 *
 * This is the meaning of the flags:
 *
 * w: write command (may modify the key space).
 * r: read command  (will never modify the key space).
 * m: may increase memory usage once called. Don't allow if out of memory.
 * a: admin command, like SAVE or SHUTDOWN.
 * p: Pub/Sub related command.
 * f: force replication of this command, regardless of server.dirty.
 * s: command not allowed in scripts.
 * R: random command. Command is not deterministic, that is, the same command
 *    with the same arguments, with the same key space, may have different
 *    results. For instance SPOP and RANDOMKEY are two random commands.
 * S: Sort command output array if called from script, so that the output
 *    is deterministic.
 * l: Allow command while loading the database.
 * t: Allow command while a slave has stale data but is not allowed to
 *    server this data. Normally no command is accepted in this condition
 *    but just a few.
 * M: Do not automatically propagate the command on MONITOR.
 * k: Perform an implicit ASKING for this command, so the command will be
 *    accepted in cluster mode if the slot is marked as 'importing'.
 * F: Fast command: O(1) or O(log(N)) command that should never delay
 *    its execution as long as the kernel scheduler is giving us time.
 *    Note that commands that may trigger a DEL as a side effect (like SET)
 *    are not fast commands.
 */

// 命令表
// char *name：命令的名字
// redisCommandProc *proc：命令实现的函数
// int arity：参数个数，-N表示大于等于N
// char *sflags：命令的属性，用以下字符作为标识
/*
w：写入命令，会修改数据库。
r：读取命令，不会修改数据库。
m：一旦执行会增加内存使用，如果内存短缺则不被允许执行。
a：管理员命令，例如：SAVE or SHUTDOWN。
p：发布订阅有关的命令。
f：强制进行复制的命令，无视服务器的脏键。
s：不能在脚本中执行的命令。
R：随机命令。相同的键有相同的参数，在相同的数据库中，可能会有不同的结果。
S：如果在脚本中调用，那么会对这个命令的输出进行一次排序。
l：当载入数据库时，允许执行该命令。
t：从节点服务器持有过期数据时，允许执行的命令。
M：不能在 MONITOR 下自动传播的命令。
k：为该命令执行一个隐式的 ASKING，所以在集群模式下，如果槽被标记为'importing'，那这个命令会被接收。
F：快速执行的命令。时间复杂度为O(1) or O(log(N))的命令只要内核调度为Redis分配时间片，那么就不应该在执行时被延迟
*/
// int flags：sflags的二进制标识形式，可以通过位运算进行组合
// redisGetKeysProc *getkeys_proc：从命令中获取键的参数，是一个可选的功能，一般用于三个字段不够执行键的参数的情况。
// int firstkey：第一个参数是 key
// int lastkey：最后一个参数是 key
// int keystep：从第一个 key 到最后一个 key 的步长。MSET 的步长是 2 因为：key,val,key,val,...
// long long microseconds：记录执行命令的耗费总时长
// long long calls：记录命令被执行的总次数
struct redisCommand redisCommandTable[] = {
	{"get",getCommand,2,"rF",0,NULL,1,1,1,0,0},
	{"set",setCommand,-3,"wm",0,NULL,1,1,1,0,0}
};


struct evictionPoolEntry *evictionPoolAlloc(void);

/*============================ Utility functions ============================ */


/* Low level logging. To use only for very big messages, otherwise
 * serverLog() is to prefer. */
void serverLogRaw(int level, const char *msg) {
	const int syslogLevelMap[] = { LOG_DEBUG, LOG_INFO, LOG_NOTICE, LOG_WARNING };
	const char *c = ".-*#";
	FILE *fp;
	char buf[64];
	int rawmode = (level & LL_RAW);
	int log_to_stdout = server.logfile[0] == '\0';

	level &= 0xff; /* clear flags */
	if (level < server.verbosity) return;

	fp = log_to_stdout ? stdout : fopen(server.logfile,"a");
	if (!fp) return;

	if (rawmode) {
		fprintf(fp,"%s",msg);
	} else {
		int off;
		struct timeval tv;
		int role_char;
		pid_t pid = getpid();

		gettimeofday(&tv,NULL);
		off = strftime(buf,sizeof(buf),"%d %b %H:%M:%S.",localtime(&tv.tv_sec));
		snprintf(buf+off,sizeof(buf)-off,"%03d",(int)tv.tv_usec/1000);
		if (server.sentinel_mode) {
			role_char = 'X'; /* Sentinel. */
		} else if (pid != server.pid) {
			role_char = 'C'; /* RDB / AOF writing child. */
		} else {
			role_char = (server.masterhost ? 'S':'M'); /* Slave or Master. */
		}
		fprintf(fp,"%d:%c %s %c %s\n",
				(int)getpid(),role_char, buf,c[level],msg);
	}
	fflush(fp);

	if (!log_to_stdout) fclose(fp);
	if (server.syslog_enabled) syslog(syslogLevelMap[level], "%s", msg);
}

/* Like serverLogRaw() but with printf-alike support. This is the function that
 * is used across the code. The raw version is only used in order to dump
 * the INFO output on crash. */
void serverLog(int level, const char *fmt, ...) {
	va_list ap;
	char msg[LOG_MAX_LEN];

	if ((level&0xff) < server.verbosity) return;

	va_start(ap, fmt);
	vsnprintf(msg, sizeof(msg), fmt, ap);
	va_end(ap);

	serverLogRaw(level,msg);
}

/* Return the UNIX time in microseconds */
long long ustime(void) {
	struct timeval tv;
	long long ust;

	gettimeofday(&tv, NULL);
	ust = ((long long)tv.tv_sec)*1000000;
	ust += tv.tv_usec;
	return ust;
}

/* Return the UNIX time in milliseconds */
mstime_t mstime(void) {
	return ustime()/1000;
}

/*====================== Hash table type implementation  ==================== */

void dictListDestructor(void *privdata, void *val)
{
    DICT_NOTUSED(privdata);
    listRelease((list*)val);
}

// 封装字典的两个sds比较方法
int dictSdsKeyCompare(void *privdata, const void *key1,
		const void *key2)
{
	int l1,l2;
	DICT_NOTUSED(privdata);

	l1 = sdslen((sds)key1);
	l2 = sdslen((sds)key2);
	if (l1 != l2) return 0;
	return memcmp(key1, key2, l1) == 0;
}




/* A case insensitive version used for the command lookup table and other
 * places where case insensitive non binary-safe comparison is needed. */
int dictSdsKeyCaseCompare(void *privdata, const void *key1,
		const void *key2)
{
	DICT_NOTUSED(privdata);

	return strcasecmp(key1, key2) == 0;
}



// 封装字典的对象释放方法
void dictObjectDestructor(void *privdata, void *val)
{
	DICT_NOTUSED(privdata);

	if (val == NULL) return; /* Values of swapped out keys as set to NULL */
	decrRefCount(val);
}

// 封装字典的sds释放方法
void dictSdsDestructor(void *privdata, void *val)
{
	DICT_NOTUSED(privdata);
	sdsfree(val);
}

// 将对象解码成字符串进行比较的函数
int dictEncObjKeyCompare(void *privdata, const void *key1,
		const void *key2)
{
	robj *o1 = (robj*) key1, *o2 = (robj*) key2;
	int cmp;

	if (o1->encoding == OBJ_ENCODING_INT &&
			o2->encoding == OBJ_ENCODING_INT)
		return o1->ptr == o2->ptr;

	o1 = getDecodedObject(o1);
	o2 = getDecodedObject(o2);
	cmp = dictSdsKeyCompare(privdata,o1->ptr,o2->ptr);
	decrRefCount(o1);
	decrRefCount(o2);
	return cmp;
}

// 封装字典的对象值比较方法
int dictObjKeyCompare(void *privdata, const void *key1,
        const void *key2)
{
    const robj *o1 = key1, *o2 = key2;
    return dictSdsKeyCompare(privdata,o1->ptr,o2->ptr);
}

unsigned int dictObjHash(const void *key) {
    const robj *o = key;
    return dictGenHashFunction(o->ptr, sdslen((sds)o->ptr));
}

// 计算字典的sds哈希值函数
unsigned int dictSdsHash(const void *key) {
	return dictGenHashFunction((unsigned char*)key, sdslen((char*)key));
}

// 计算字典的sds哈希值函数，另一种比较 insensitive 的 hash function
unsigned int dictSdsCaseHash(const void *key) {
	return dictGenCaseHashFunction((unsigned char*)key, sdslen((char*)key));
}

// 计算key的哈希值
unsigned int dictEncObjHash(const void *key) {
	robj *o = (robj*) key;

	if (sdsEncodedObject(o)) {
		return dictGenHashFunction(o->ptr, sdslen((sds)o->ptr));
	} else {
		if (o->encoding == OBJ_ENCODING_INT) {
			char buf[32];
			int len;

			len = ll2string(buf,32,(long)o->ptr);
			return dictGenHashFunction((unsigned char*)buf, len);
		} else {
			unsigned int hash;

			o = getDecodedObject(o);
			hash = dictGenHashFunction(o->ptr, sdslen((sds)o->ptr));
			decrRefCount(o);
			return hash;
		}
	}
}


// 根据以上封装不同的函数，来初始化不同的情况下的字典类型的特定函数
/* Sets type hash table */
dictType setDictType = {
	dictEncObjHash,            /* hash function */
	NULL,                      /* key dup */
	NULL,                      /* val dup */
	dictEncObjKeyCompare,      /* key compare */
	dictObjectDestructor, /* key destructor */
	NULL                       /* val destructor */
};


/* Sorted sets hash (note: a skiplist is used in addition to the hash table) */
dictType zsetDictType = {
	dictEncObjHash,            /* hash function */
	NULL,                      /* key dup */
	NULL,                      /* val dup */
	dictEncObjKeyCompare,      /* key compare */
	dictObjectDestructor, /* key destructor */
	NULL                       /* val destructor */
};


/* Db->dict, keys are sds strings, vals are Redis objects. */
dictType dbDictType = {
	dictSdsHash,                /* hash function */
	NULL,                       /* key dup */
	NULL,                       /* val dup */
	dictSdsKeyCompare,          /* key compare */
	dictSdsDestructor,          /* key destructor */
	dictObjectDestructor   /* val destructor */
};

/* Db->expires */
dictType keyptrDictType = {
	dictSdsHash,               /* hash function */
	NULL,                      /* key dup */
	NULL,                      /* val dup */
	dictSdsKeyCompare,         /* key compare */
	NULL,                      /* key destructor */
	NULL                       /* val destructor */
};

/* Command table. sds string -> command struct pointer. */
dictType commandTableDictType = {
	dictSdsCaseHash,           /* hash function */
	NULL,                      /* key dup */
	NULL,                      /* val dup */
	dictSdsKeyCaseCompare,     /* key compare */
	dictSdsDestructor,         /* key destructor */
	NULL                       /* val destructor */
};


/* Hash type hash table (note that small hashes are represented with ziplists) */
dictType hashDictType = {
	dictEncObjHash,             /* hash function */
	NULL,                       /* key dup */
	NULL,                       /* val dup */
	dictEncObjKeyCompare,       /* key compare */
	dictObjectDestructor,  /* key destructor */
	dictObjectDestructor   /* val destructor */
};


/* Keylist hash table type has unencoded redis objects as keys and
 * lists as values. It's used for blocking operations (BLPOP) and to
 * map swapped keys to a list of clients waiting for this keys to be loaded. */
dictType keylistDictType = {
    dictObjHash,                /* hash function */
    NULL,                       /* key dup */
    NULL,                       /* val dup */
    dictObjKeyCompare,          /* key compare */
    dictObjectDestructor,  /* key destructor */
    dictListDestructor          /* val destructor */
};



/* Migrate cache dict type. */
dictType migrateCacheDictType = {
	dictSdsHash,                /* hash function */
	NULL,                       /* key dup */
	NULL,                       /* val dup */
	dictSdsKeyCompare,          /* key compare */
	dictSdsDestructor,          /* key destructor */
	NULL                        /* val destructor */
};

unsigned int getLRUClock(void) {
	return (mstime()/LRU_CLOCK_RESOLUTION) & LRU_CLOCK_MAX;
}

void updateCachedTime(void) {
    server.unixtime = time(NULL);
    server.mstime = mstime();
}


int serverCron(struct aeEventLoop *eventLoop, long long id, void *clientData) {
    int j;
    UNUSED(eventLoop);
    UNUSED(id);
    UNUSED(clientData);

//TODO


    // 返回周期，默认为100ms
    return 1000/server.hz;
}



// 在Redis进入事件循环之前被调用
void beforeSleep(struct aeEventLoop *eventLoop) {
    UNUSED(eventLoop);

    //// 在sleep函数之前调用 clusterBeforeSleep()。
    //// 请注意，此功能可能会更改Redis Cluster的状态（从ok到fail，反之亦然）
    //if (server.cluster_enabled) clusterBeforeSleep();

    //// 主节点主动执行过期键的删除操作，以快速模式执行，1ms
    //if (server.active_expire_enabled && server.masterhost == NULL)
    //    activeExpireCycle(ACTIVE_EXPIRE_CYCLE_FAST);

    //// 如果至少一个client在进入事件循环之前被阻塞，那么发送所有的从节点一个ack请求
    //// get_ack_from_slaves如果为真，则发送REPLCONF GETACK
    //if (server.get_ack_from_slaves) {
    //    robj *argv[3];
    //    // 创建一个参数对象列表
    //    argv[0] = createStringObject("REPLCONF",8);
    //    argv[1] = createStringObject("GETACK",6);
    //    argv[2] = createStringObject("*",1); /* Not used argument. */
    //    // 给所有从节点服务器发送该请求
    //    replicationFeedSlaves(server.slaves, server.slaveseldb, argv, 3);
    //    // 释放参数对象列表
    //    decrRefCount(argv[0]);
    //    decrRefCount(argv[1]);
    //    decrRefCount(argv[2]);
    //    // 清空标志
    //    server.get_ack_from_slaves = 0;
    //}

    ///* Unblock all the clients blocked for synchronous replication
    // * in WAIT. */
    //// 解除所有等待WAIT命令而被阻塞的client
    //if (listLength(server.clients_waiting_acks))
    //    processClientsWaitingReplicas();

    ///* Try to process pending commands for clients that were just unblocked. */
    //// 处理所有非阻塞的client的输入缓冲区的内容
    //if (listLength(server.unblocked_clients))
    //    processUnblockedClients();

    ///* Write the AOF buffer on disk */
    //// 将AOF缓存冲洗到磁盘中
    //flushAppendOnlyFile(0);

    /* Handle writes with pending output buffers. */
    // 处理放在clients_pending_write链表中的待写的client，将输出缓冲区的内容写到fd中
    handleClientsWithPendingWrites();
}//end of beforeSleep()



// 创建共享对象
void createSharedObjects(void) {
	int j;

	shared.crlf = createObject(OBJ_STRING,sdsnew("\r\n"));
	shared.ok = createObject(OBJ_STRING,sdsnew("+OK\r\n"));
	shared.err = createObject(OBJ_STRING,sdsnew("-ERR\r\n"));
	shared.emptybulk = createObject(OBJ_STRING,sdsnew("$0\r\n\r\n"));
	shared.czero = createObject(OBJ_STRING,sdsnew(":0\r\n"));
	shared.cone = createObject(OBJ_STRING,sdsnew(":1\r\n"));
	shared.cnegone = createObject(OBJ_STRING,sdsnew(":-1\r\n"));
	shared.nullbulk = createObject(OBJ_STRING,sdsnew("$-1\r\n"));
	shared.nullmultibulk = createObject(OBJ_STRING,sdsnew("*-1\r\n"));
	shared.emptymultibulk = createObject(OBJ_STRING,sdsnew("*0\r\n"));
	shared.pong = createObject(OBJ_STRING,sdsnew("+PONG\r\n"));
	shared.queued = createObject(OBJ_STRING,sdsnew("+QUEUED\r\n"));
	shared.emptyscan = createObject(OBJ_STRING,sdsnew("*2\r\n$1\r\n0\r\n*0\r\n"));
	shared.wrongtypeerr = createObject(OBJ_STRING,sdsnew(
				"-WRONGTYPE Operation against a key holding the wrong kind of value\r\n"));
	shared.nokeyerr = createObject(OBJ_STRING,sdsnew(
				"-ERR no such key\r\n"));
	shared.syntaxerr = createObject(OBJ_STRING,sdsnew(
				"-ERR syntax error\r\n"));
	shared.sameobjecterr = createObject(OBJ_STRING,sdsnew(
				"-ERR source and destination objects are the same\r\n"));
	shared.outofrangeerr = createObject(OBJ_STRING,sdsnew(
				"-ERR index out of range\r\n"));
	shared.noscripterr = createObject(OBJ_STRING,sdsnew(
				"-NOSCRIPT No matching script. Please use EVAL.\r\n"));
	shared.loadingerr = createObject(OBJ_STRING,sdsnew(
				"-LOADING Redis is loading the dataset in memory\r\n"));
	shared.slowscripterr = createObject(OBJ_STRING,sdsnew(
				"-BUSY Redis is busy running a script. You can only call SCRIPT KILL or SHUTDOWN NOSAVE.\r\n"));
	shared.masterdownerr = createObject(OBJ_STRING,sdsnew(
				"-MASTERDOWN Link with MASTER is down and slave-serve-stale-data is set to 'no'.\r\n"));
	shared.bgsaveerr = createObject(OBJ_STRING,sdsnew(
				"-MISCONF Redis is configured to save RDB snapshots, but is currently not able to persist on disk. Commands that may modify the data set are disabled. Please check Redis logs for details about the error.\r\n"));
	shared.roslaveerr = createObject(OBJ_STRING,sdsnew(
				"-READONLY You can't write against a read only slave.\r\n"));
	shared.noautherr = createObject(OBJ_STRING,sdsnew(
				"-NOAUTH Authentication required.\r\n"));
	shared.oomerr = createObject(OBJ_STRING,sdsnew(
				"-OOM command not allowed when used memory > 'maxmemory'.\r\n"));
	shared.execaborterr = createObject(OBJ_STRING,sdsnew(
				"-EXECABORT Transaction discarded because of previous errors.\r\n"));
	shared.noreplicaserr = createObject(OBJ_STRING,sdsnew(
				"-NOREPLICAS Not enough good slaves to write.\r\n"));
	shared.busykeyerr = createObject(OBJ_STRING,sdsnew(
				"-BUSYKEY Target key name already exists.\r\n"));
	shared.space = createObject(OBJ_STRING,sdsnew(" "));
	shared.colon = createObject(OBJ_STRING,sdsnew(":"));
	shared.plus = createObject(OBJ_STRING,sdsnew("+"));

	for (j = 0; j < PROTO_SHARED_SELECT_CMDS; j++) {
		char dictid_str[64];
		int dictid_len;
		dictid_len = ll2string(dictid_str,sizeof(dictid_str),j);
		shared.select[j] = createObject(OBJ_STRING,
				sdscatprintf(sdsempty(),
					"*2\r\n$6\r\nSELECT\r\n$%d\r\n%s\r\n",
					dictid_len, dictid_str));
	}

    shared.messagebulk = createStringObject("$7\r\nmessage\r\n",13);
    shared.pmessagebulk = createStringObject("$8\r\npmessage\r\n",14);
    shared.subscribebulk = createStringObject("$9\r\nsubscribe\r\n",15);
    shared.unsubscribebulk = createStringObject("$11\r\nunsubscribe\r\n",18);
    shared.psubscribebulk = createStringObject("$10\r\npsubscribe\r\n",17);
    shared.punsubscribebulk = createStringObject("$12\r\npunsubscribe\r\n",19);
    shared.del = createStringObject("DEL",3);
    shared.rpop = createStringObject("RPOP",4);
    shared.lpop = createStringObject("LPOP",4);
    shared.lpush = createStringObject("LPUSH",5);

	// 共享的整数对象[0-9999]
	for (j = 0; j < OBJ_SHARED_INTEGERS; j++) {
		shared.integers[j] = createObject(OBJ_STRING,(void*)(long)j);
		shared.integers[j]->encoding = OBJ_ENCODING_INT;
	}

    for (j = 0; j < OBJ_SHARED_BULKHDR_LEN; j++) {
        shared.mbulkhdr[j] = createObject(OBJ_STRING,
            sdscatprintf(sdsempty(),"*%d\r\n",j));
        shared.bulkhdr[j] = createObject(OBJ_STRING,
            sdscatprintf(sdsempty(),"$%d\r\n",j));
    }

    /* The following two shared objects, minstring and maxstrings, are not
     * actually used for their value but as a special object meaning
     * respectively the minimum possible string and the maximum possible
     * string in string comparisons for the ZRANGEBYLEX command. */
    shared.minstring = createStringObject("minstring",9);
    shared.maxstring = createStringObject("maxstring",9);
}//end of createSharedObjects(void);

// 初始化服务器配置
void initServerConfig(void) {
	int j;
	// 长度为40字节的服务器ID
	getRandomHexChars(server.runid,CONFIG_RUN_ID_SIZE);
	server.configfile = NULL;
	server.executable = NULL;
	server.hz = CONFIG_DEFAULT_HZ;      // 10
	server.runid[CONFIG_RUN_ID_SIZE] = '\0';
	server.arch_bits = (sizeof(long) == 8) ? 64 : 32;
	server.port = CONFIG_DEFAULT_SERVER_PORT;
	server.tcp_backlog = CONFIG_DEFAULT_TCP_BACKLOG;
	server.bindaddr_count = 0;
	server.unixsocket = NULL;
	server.unixsocketperm = CONFIG_DEFAULT_UNIX_SOCKET_PERM;
	server.ipfd_count = 0;
	server.sofd = -1;
	server.protected_mode = CONFIG_DEFAULT_PROTECTED_MODE;
	server.dbnum = CONFIG_DEFAULT_DBNUM;
	server.verbosity = CONFIG_DEFAULT_VERBOSITY;
	server.maxidletime = CONFIG_DEFAULT_CLIENT_TIMEOUT;
	server.tcpkeepalive = CONFIG_DEFAULT_TCP_KEEPALIVE;
	server.active_expire_enabled = 1;
	server.client_max_querybuf_len = PROTO_MAX_QUERYBUF_LEN;
	server.saveparams = NULL;
	server.loading = 0;
	server.logfile = zstrdup(CONFIG_DEFAULT_LOGFILE);
	server.syslog_enabled = CONFIG_DEFAULT_SYSLOG_ENABLED;
	server.syslog_ident = zstrdup(CONFIG_DEFAULT_SYSLOG_IDENT);
	server.syslog_facility = LOG_LOCAL0;
	server.daemonize = CONFIG_DEFAULT_DAEMONIZE;
	server.supervised = 0;
	server.supervised_mode = SUPERVISED_NONE;
	server.aof_state = AOF_OFF;
	server.aof_fsync = CONFIG_DEFAULT_AOF_FSYNC;
	server.aof_no_fsync_on_rewrite = CONFIG_DEFAULT_AOF_NO_FSYNC_ON_REWRITE;
	server.aof_rewrite_perc = AOF_REWRITE_PERC;
	server.aof_rewrite_min_size = AOF_REWRITE_MIN_SIZE;
	server.aof_rewrite_base_size = 0;
	server.aof_rewrite_scheduled = 0;
	server.aof_last_fsync = time(NULL);
	server.aof_rewrite_time_last = -1;
	server.aof_rewrite_time_start = -1;
	server.aof_lastbgrewrite_status = C_OK;
	server.aof_delayed_fsync = 0;
	server.aof_fd = -1;
	server.aof_selected_db = -1; /* Make sure the first time will not match */
	server.aof_flush_postponed_start = 0;
	server.aof_rewrite_incremental_fsync = CONFIG_DEFAULT_AOF_REWRITE_INCREMENTAL_FSYNC;
	server.aof_load_truncated = CONFIG_DEFAULT_AOF_LOAD_TRUNCATED;
	server.pidfile = NULL;
	server.rdb_filename = zstrdup(CONFIG_DEFAULT_RDB_FILENAME);
	server.aof_filename = zstrdup(CONFIG_DEFAULT_AOF_FILENAME);
	server.requirepass = NULL;
	server.rdb_compression = CONFIG_DEFAULT_RDB_COMPRESSION;
	server.rdb_checksum = CONFIG_DEFAULT_RDB_CHECKSUM;
	server.stop_writes_on_bgsave_err = CONFIG_DEFAULT_STOP_WRITES_ON_BGSAVE_ERROR;
	server.activerehashing = CONFIG_DEFAULT_ACTIVE_REHASHING;
	server.notify_keyspace_events = 0;
	server.maxclients = CONFIG_DEFAULT_MAX_CLIENTS;
	server.bpop_blocked_clients = 0;
	server.maxmemory = CONFIG_DEFAULT_MAXMEMORY;
	server.maxmemory_policy = CONFIG_DEFAULT_MAXMEMORY_POLICY;
	server.maxmemory_samples = CONFIG_DEFAULT_MAXMEMORY_SAMPLES;

	//超过任何值一个值hash的底层就从ziplist变成dict
	//server.hash_max_ziplist_entries = OBJ_HASH_MAX_ZIPLIST_ENTRIES;//512 hash类型下ziplist中最多能存放的节点数量
	server.hash_max_ziplist_entries = 5;//512 hash类型下ziplist中最多能存放的节点数量
	server.hash_max_ziplist_value = OBJ_HASH_MAX_ZIPLIST_VALUE;//64 hash类型下ziplist中最大能存放的值长度

	server.list_max_ziplist_size = OBJ_LIST_MAX_ZIPLIST_SIZE;
	server.list_compress_depth = OBJ_LIST_COMPRESS_DEPTH;

	//server.set_max_intset_entries = OBJ_SET_MAX_INTSET_ENTRIES;//512
	server.set_max_intset_entries = 5;//set 保存数据超过5个就会转换从intset成为hashtable

	//server.zset_max_ziplist_entries = OBJ_ZSET_MAX_ZIPLIST_ENTRIES;//128
	server.zset_max_ziplist_entries = 5;//128
	server.zset_max_ziplist_value = OBJ_ZSET_MAX_ZIPLIST_VALUE;//64

	server.hll_sparse_max_bytes = CONFIG_DEFAULT_HLL_SPARSE_MAX_BYTES;
	server.shutdown_asap = 0;
	server.repl_ping_slave_period = CONFIG_DEFAULT_REPL_PING_SLAVE_PERIOD;
	server.repl_timeout = CONFIG_DEFAULT_REPL_TIMEOUT;
	server.repl_min_slaves_to_write = CONFIG_DEFAULT_MIN_SLAVES_TO_WRITE;
	server.repl_min_slaves_max_lag = CONFIG_DEFAULT_MIN_SLAVES_MAX_LAG;
	server.cluster_enabled = 0;
	server.cluster_node_timeout = CLUSTER_DEFAULT_NODE_TIMEOUT;
	server.cluster_migration_barrier = CLUSTER_DEFAULT_MIGRATION_BARRIER;
	server.cluster_slave_validity_factor = CLUSTER_DEFAULT_SLAVE_VALIDITY;
	server.cluster_require_full_coverage = CLUSTER_DEFAULT_REQUIRE_FULL_COVERAGE;
	server.cluster_configfile = zstrdup(CONFIG_DEFAULT_CLUSTER_CONFIG_FILE);
	server.migrate_cached_sockets = dictCreate(&migrateCacheDictType,NULL);
	server.next_client_id = 1; /* Client IDs, start from 1 .*/
	server.loading_process_events_interval_bytes = (1024*1024*2);

	server.lruclock = getLRUClock();
	resetServerSaveParams();
	// SAVE的参数
	appendServerSaveParams(60*60,1);  /* save after 1 hour and 1 change */
	appendServerSaveParams(300,100);  /* save after 5 minutes and 100 changes */
	appendServerSaveParams(60,10000); /* save after 1 minute and 10000 changes */
	/* Replication related */
	server.masterauth = NULL;
	server.masterhost = NULL;
	server.masterport = 6379;
	server.master = NULL;
	server.cached_master = NULL;
	server.repl_master_initial_offset = -1;
	server.repl_state = REPL_STATE_NONE;
	server.repl_syncio_timeout = CONFIG_REPL_SYNCIO_TIMEOUT;
	server.repl_serve_stale_data = CONFIG_DEFAULT_SLAVE_SERVE_STALE_DATA;
	server.repl_slave_ro = CONFIG_DEFAULT_SLAVE_READ_ONLY;
	server.repl_down_since = 0; /* Never connected, repl is down since EVER. */
	server.repl_disable_tcp_nodelay = CONFIG_DEFAULT_REPL_DISABLE_TCP_NODELAY;
	server.repl_diskless_sync = CONFIG_DEFAULT_REPL_DISKLESS_SYNC;
	server.repl_diskless_sync_delay = CONFIG_DEFAULT_REPL_DISKLESS_SYNC_DELAY;
	server.slave_priority = CONFIG_DEFAULT_SLAVE_PRIORITY;
	server.slave_announce_ip = CONFIG_DEFAULT_SLAVE_ANNOUNCE_IP;
	server.slave_announce_port = CONFIG_DEFAULT_SLAVE_ANNOUNCE_PORT;
	server.master_repl_offset = 0;

	/* Replication partial resync backlog */
	server.repl_backlog = NULL;
	server.repl_backlog_size = CONFIG_DEFAULT_REPL_BACKLOG_SIZE;
	server.repl_backlog_histlen = 0;
	server.repl_backlog_idx = 0;
	server.repl_backlog_off = 0;
	server.repl_backlog_time_limit = CONFIG_DEFAULT_REPL_BACKLOG_TIME_LIMIT;
	server.repl_no_slaves_since = time(NULL);

	/* Client output buffer limits */
	for (j = 0; j < CLIENT_TYPE_OBUF_COUNT; j++)
		server.client_obuf_limits[j] = clientBufferLimitsDefaults[j];

	/* Double constants initialization */
	// 全局double类型常量实现
	R_Zero = 0.0;
	R_PosInf = 1.0/R_Zero;
	R_NegInf = -1.0/R_Zero;
	R_Nan = R_Zero/R_Zero;

	/* Command table -- we initiialize it here as it is part of the
	 * initial configuration, since command names may be changed via
	 * redis.conf using the rename-command directive. */
	// 初始化命令表
	server.commands = dictCreate(&commandTableDictType,NULL);
	server.orig_commands = dictCreate(&commandTableDictType,NULL);
	populateCommandTable();
	//server.delCommand = lookupCommandByCString("del");
	//server.multiCommand = lookupCommandByCString("multi");
	//server.lpushCommand = lookupCommandByCString("lpush");
	//server.lpopCommand = lookupCommandByCString("lpop");
	//server.rpopCommand = lookupCommandByCString("rpop");
	//server.sremCommand = lookupCommandByCString("srem");
	//server.execCommand = lookupCommandByCString("exec");

	/* Slow log */
	server.slowlog_log_slower_than = CONFIG_DEFAULT_SLOWLOG_LOG_SLOWER_THAN;
	server.slowlog_max_len = CONFIG_DEFAULT_SLOWLOG_MAX_LEN;

	/* Latency monitor */
	server.latency_monitor_threshold = CONFIG_DEFAULT_LATENCY_MONITOR_THRESHOLD;

	/* Debugging */
	server.assert_failed = "<no assertion failed>";
	server.assert_file = "<no file>";
	server.assert_line = 0;
	server.bug_report_start = 0;
	server.watchdog_period = 0;

}//end of initServerConfig()




/* This function will try to raise the max number of open files accordingly to
 * the configured max number of clients. It also reserves a number of file
 * descriptors (CONFIG_MIN_RESERVED_FDS) for extra operations of
 * persistence, listening sockets, log files and so forth.
 *
 * If it will not be possible to set the limit accordingly to the configured
 * max number of clients, the function will do the reverse setting
 * server.maxclients to the value that we can actually handle. */
// 此功能将尝试根据配置的最大客户端数量提高打开文件的最大数量。 它还保留了许多文件描述符（CONFIG_MIN_RESERVED_FDS）用于持久化，监听套接字，日志文件等的额外操作
// 如果不能将限制相应地设置为配置的最大客户端数量，则该功能将反向设置server.maxclients为我们实际可以处理的值
void adjustOpenFilesLimit(void) {
	// 最大的fd数量
	rlim_t maxfiles = server.maxclients+CONFIG_MIN_RESERVED_FDS;
	struct rlimit limit;

	// 获取系统最多打开的文件数
	if (getrlimit(RLIMIT_NOFILE,&limit) == -1) {
		serverLog(LL_WARNING,"Unable to obtain the current NOFILE limit (%s), \
				assuming 1024 and setting the max clients configuration accordingly.",
				strerror(errno));
		server.maxclients = 1024-CONFIG_MIN_RESERVED_FDS;
	} else {
		// 获取系统的软限制
		rlim_t oldlimit = limit.rlim_cur;

		/* Set the max number of files if the current limit is not enough
		 * for our needs. */
		// 当前最大fd数已经超过软限制
		if (oldlimit < maxfiles) {
			rlim_t bestlimit;
			int setrlimit_error = 0;

			/* Try to set the file limit to match 'maxfiles' or at least
			 * to the higher value supported less than maxfiles. */
			bestlimit = maxfiles;
			// 设置打开文件个数限制为maxfiles或是小于maxfiles的最大值
			while(bestlimit > oldlimit) {
				rlim_t decr_step = 16;

				limit.rlim_cur = bestlimit;
				limit.rlim_max = bestlimit;
				// 设置为maxfiles
				if (setrlimit(RLIMIT_NOFILE,&limit) != -1) break;
				setrlimit_error = errno;

				/* We failed to set file limit to 'bestlimit'. Try with a
				 * smaller limit decrementing by a few FDs per iteration. */
				if (bestlimit < decr_step) break;
				// 每次减16，下次设置小于maxfiles的最大值
				bestlimit -= decr_step;
			}

			/* Assume that the limit we get initially is still valid if
			 * our last try was even lower. */
			// 如果我们的最后一次尝试更低的bestlimit，那么我们最初得到的限制仍然有效
			if (bestlimit < oldlimit) bestlimit = oldlimit;

			// 当bestlimit被设置为小于maxfiles的最大值，更新服务器的maxclients，并打印日志
			if (bestlimit < maxfiles) {
				int old_maxclients = server.maxclients;
				server.maxclients = bestlimit-CONFIG_MIN_RESERVED_FDS;
				if (server.maxclients < 1) {
					serverLog(LL_WARNING,"Your current 'ulimit -n' "
							"of %llu is not enough for the server to start. "
							"Please increase your open file limit to at least "
							"%llu. Exiting.",
							(unsigned long long) oldlimit,
							(unsigned long long) maxfiles);
					exit(1);
				}
				serverLog(LL_WARNING,"You requested maxclients of %d "
						"requiring at least %llu max file descriptors.",
						old_maxclients,
						(unsigned long long) maxfiles);
				serverLog(LL_WARNING,"Server can't set maximum open files "
						"to %llu because of OS error: %s.",
						(unsigned long long) maxfiles, strerror(setrlimit_error));
				serverLog(LL_WARNING,"Current maximum open files is %llu. "
						"maxclients has been reduced to %d to compensate for "
						"low ulimit. "
						"If you need higher maxclients increase 'ulimit -n'.",
						(unsigned long long) bestlimit, server.maxclients);
			} else {
				serverLog(LL_NOTICE,"Increased maximum number of open files "
						"to %llu (it was originally set to %llu).",
						(unsigned long long) maxfiles,
						(unsigned long long) oldlimit);
			}
		}
	}
}//end of adjustOpenFilesLimit(void);



/* Check that server.tcp_backlog can be actually enforced in Linux according
 * to the value of /proc/sys/net/core/somaxconn, or warn about it. */
// 检查server.tcp_backlog是否可以根据 /proc/sys/net/core/somaxconn 的值在Linux中实际执行，或者是对它进行警告
void checkTcpBacklogSettings(void) {
	// Linux环境才会执行
#ifdef HAVE_PROC_SOMAXCONN
	// 打开该文件
	FILE *fp = fopen("/proc/sys/net/core/somaxconn","r");
	char buf[1024];
	if (!fp) return;
	// 读出数值
	if (fgets(buf,sizeof(buf),fp) != NULL) {
		int somaxconn = atoi(buf);
		// 如果服务器的tcp_backlog大于系统的backlog的128，打印日志
		if (somaxconn > 0 && somaxconn < server.tcp_backlog) {
			serverLog(LL_WARNING,"WARNING: The TCP backlog setting of %d cannot be enforced because /proc/sys/net/core/somaxconn is set to the lower value of %d.", server.tcp_backlog, somaxconn);
		}
	}
	fclose(fp);
#endif
}

// 初始化一组文件描述符以监听指定的 'port'，该'port'绑定Redis服务器配置中的地址
// 监听的文件描述符被排序在整型数组 'fds' 中，并且设置为'*count'
// 要绑定的地址在全局的，server.bindaddr数组中指定，并且其编号为server.bindaddr_count。
// 如果服务器的配置没有包含绑定的地址，那么这个函数将尝试绑定所有IPv4和IPv6协议的地址
// 成功，函数返回C_OK。出错时，函数返回C_ERR。
// 如果函数发生错误，那么至少有一个server.bindaddr地址无法绑定，或者在服务器配置中未指定绑定地址，但该函数无法绑定至少IPv4或IPv6协议的一个
int listenToPort(int port, int *fds, int *count) {
    int j;

    /* Force binding of 0.0.0.0 if no bind address is specified, always
     * entering the loop if j == 0. */
    // 没有制定绑定的地址，设置为空
    if (server.bindaddr_count == 0) server.bindaddr[0] = NULL;
    // 遍历绑定的地址
    for (j = 0; j < server.bindaddr_count || j == 0; j++) {
        // 没有绑定地址的情况
        if (server.bindaddr[j] == NULL) {
            int unsupported = 0;
            /* Bind * for both IPv6 and IPv4, we enter here only if
             * server.bindaddr_count == 0. */
            // 绑定 IPv6 的所有地址
            fds[*count] = anetTcp6Server(server.neterr,port,NULL,
                server.tcp_backlog);
            if (fds[*count] != ANET_ERR) {
                anetNonBlock(NULL,fds[*count]);
                (*count)++;
            } else if (errno == EAFNOSUPPORT) {
                unsupported++;
                serverLog(LL_WARNING,"Not listening to IPv6: unsupproted");
            }

            // 绑定 IPv4 的所有地址
            if (*count == 1 || unsupported) {
                /* Bind the IPv4 address as well. */
                fds[*count] = anetTcpServer(server.neterr,port,NULL,
                    server.tcp_backlog);
                if (fds[*count] != ANET_ERR) {
                    anetNonBlock(NULL,fds[*count]);
                    (*count)++;
                } else if (errno == EAFNOSUPPORT) {
                    unsupported++;
                    serverLog(LL_WARNING,"Not listening to IPv4: unsupproted");
                }
            }
            /* Exit the loop if we were able to bind * on IPv4 and IPv6,
             * otherwise fds[*count] will be ANET_ERR and we'll print an
             * error and return to the caller with an error. */
            if (*count + unsupported == 2) break;
        // 绑定 IPv6 的指定的地址
        } else if (strchr(server.bindaddr[j],':')) {
            /* Bind IPv6 address. */
            fds[*count] = anetTcp6Server(server.neterr,port,server.bindaddr[j],
                server.tcp_backlog);
        // 绑定 IPv4 的指定的地
        } else {
            /* Bind IPv4 address. */
            fds[*count] = anetTcpServer(server.neterr,port,server.bindaddr[j],
                server.tcp_backlog);
        }
        if (fds[*count] == ANET_ERR) {
            serverLog(LL_WARNING,
                "Creating Server TCP listening socket %s:%d: %s",
                server.bindaddr[j] ? server.bindaddr[j] : "*",
                port, server.neterr);
            return C_ERR;
        }
        anetNonBlock(NULL,fds[*count]);
        (*count)++;
    }
    return C_OK;
}//end of int listenToPort(int port, int *fds, int *count)

// 初始化服务器
void initServer(void) {
	int j;

	signal(SIGHUP, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	//setupSignalHandlers();

	//if (server.syslog_enabled) {
	//	openlog(server.syslog_ident, LOG_PID | LOG_NDELAY | LOG_NOWAIT,
	//			server.syslog_facility);
	//}

	// 初始化并创建数据结构
	server.pid = getpid();
	server.current_client = NULL;
	server.clients = listCreate();
	server.clients_to_close = listCreate();
	server.slaves = listCreate();
	server.monitors = listCreate();
	server.clients_pending_write = listCreate();
	server.slaveseldb = -1; /* Force to emit the first SELECT command. */
	server.unblocked_clients = listCreate();
	server.ready_keys = listCreate();
	server.clients_waiting_acks = listCreate();
	server.get_ack_from_slaves = 0;
	server.clients_paused = 0;
	server.system_memory_size = zmalloc_get_memory_size();

	createSharedObjects();

	// 根据系统调整打开文件的最大个数，设置最大客户端连接数量
	adjustOpenFilesLimit();
	// 创建事件循环的结构
	server.el = aeCreateEventLoop(server.maxclients+CONFIG_FDSET_INCR);
	server.db = zmalloc(sizeof(redisDb)*server.dbnum);

	/* Open the TCP listening socket for the user commands. */
	// 监听端口,默认情况创建2个socket一个ip4 一个ip6
	if (server.port != 0 && listenToPort(server.port,server.ipfd,&server.ipfd_count) == C_ERR)
		exit(1);

	//printf("%d\n",server.ipfd_count);//2

	/* Abort if there are no listening sockets at all. */
	if (server.ipfd_count == 0 && server.sofd < 0) {
		serverLog(LL_WARNING, "Configured to not listen anywhere, exiting.");
		exit(1);
	}

	/* Create the Redis databases, and initialize other internal state. */
	// 创建并初始化数据库
	for (j = 0; j < server.dbnum; j++) {
		server.db[j].dict = dictCreate(&dbDictType,NULL);
		server.db[j].expires = dictCreate(&keyptrDictType,NULL);
		server.db[j].blocking_keys = dictCreate(&keylistDictType,NULL);
		server.db[j].ready_keys = dictCreate(&setDictType,NULL);
		server.db[j].watched_keys = dictCreate(&keylistDictType,NULL);
		server.db[j].eviction_pool = evictionPoolAlloc();
		server.db[j].id = j;
		server.db[j].avg_ttl = 0;
	}
	// 创建订阅/发布数据结构
	server.pubsub_channels = dictCreate(&keylistDictType,NULL);
	server.pubsub_patterns = listCreate();
	listSetFreeMethod(server.pubsub_patterns,freePubsubPattern);
	listSetMatchMethod(server.pubsub_patterns,listMatchPubsubPattern);

	server.cronloops = 0;
	server.rdb_child_pid = -1;
	server.aof_child_pid = -1;
	server.rdb_child_type = RDB_CHILD_TYPE_NONE;
	server.rdb_bgsave_scheduled = 0;
	//aofRewriteBufferReset();//TODO
	server.aof_buf = sdsempty();
	server.lastsave = time(NULL); /* At startup we consider the DB saved. */
	server.lastbgsave_try = 0;    /* At startup we never tried to BGSAVE. */
	server.rdb_save_time_last = -1;
	server.rdb_save_time_start = -1;
	server.dirty = 0;
	//resetServerStats();//TODO
	/* A few stats we don't want to reset: server startup time, and peak mem. */
	server.stat_starttime = time(NULL);
	server.stat_peak_memory = 0;
	server.resident_set_size = 0;
	server.lastbgsave_status = C_OK;
	server.aof_last_write_status = C_OK;
	server.aof_last_write_errno = 0;
	server.repl_good_slaves_count = 0;
	updateCachedTime();

	// 创建一个时间事件，并安装serverCron()处理时间事件
	if(aeCreateTimeEvent(server.el, 1, serverCron, NULL, NULL) == AE_ERR) {
		serverPanic("Can't create the serverCron time event.");
		exit(1);
	}

	for (j = 0; j < server.ipfd_count; j++) {
		if (aeCreateFileEvent(server.el, server.ipfd[j], AE_READABLE,
					acceptTcpHandler,NULL) == AE_ERR)
		{
			serverPanic(
					"Unrecoverable error creating server.ipfd file event.");
		}
	}

	//// 为Unix本地连接创建文件事件，安装acceptUnixHandler()函数来处理本地连接
	//if (server.sofd > 0 && aeCreateFileEvent(server.el,server.sofd,AE_READABLE, acceptUnixHandler,NULL) == AE_ERR) 
	//	serverPanic("Unrecoverable error creating server.sofd file event.");

	///* Open the AOF file if needed. */
	//// 按需创建AOF的文件
	//if (server.aof_state == AOF_ON) {
	//    server.aof_fd = open(server.aof_filename,
	//                           O_WRONLY|O_APPEND|O_CREAT,0644);
	//    if (server.aof_fd == -1) {
	//        serverLog(LL_WARNING, "Can't open the append-only file: %s",
	//            strerror(errno));
	//        exit(1);
	//    }
	//}



	/* 32 bit instances are limited to 4GB of address space, so if there is
	 * no explicit limit in the user provided configuration we set a limit
	 * at 3 GB using maxmemory with 'noeviction' policy'. This avoids
	 * useless crashes of the Redis instance for out of memory. */
	// 32位系统，设置最大的内存使用量
	if (server.arch_bits == 32 && server.maxmemory == 0) {
		serverLog(LL_WARNING,"Warning: 32 bit instance detected but no memory limit set. Setting 3 GB maxmemory limit with 'noeviction' policy now.");
		server.maxmemory = 3072LL*(1024*1024); /* 3 GB */
		server.maxmemory_policy = MAXMEMORY_NO_EVICTION;
	}

	//// 集群开启，则初始化cluster
	//if (server.cluster_enabled) clusterInit();
	//// 初始化复制脚本缓存
	//replicationScriptCacheInit();
	//// 初始化lua脚本系统
	//scriptingInit(1);
	//// 初始化慢查询
	//slowlogInit();
	//// 初始化延迟诊断的功能
	//latencyMonitorInit();
	//// 初始化后台IO
	//bioInit();
}//initServer()

/* Populates the Redis Command Table starting from the hard coded list
 * we have on top of redis.c file. */
void populateCommandTable(void) {
	int j;
	int numcommands = sizeof(redisCommandTable)/sizeof(struct redisCommand);

	for (j = 0; j < numcommands; j++) {
		struct redisCommand *c = redisCommandTable+j;
		char *f = c->sflags;
		int retval1, retval2;

		while(*f != '\0') {
			switch(*f) {
				case 'w': c->flags |= CMD_WRITE; break;
				case 'r': c->flags |= CMD_READONLY; break;
				case 'm': c->flags |= CMD_DENYOOM; break;
				case 'a': c->flags |= CMD_ADMIN; break;
				case 'p': c->flags |= CMD_PUBSUB; break;
				case 's': c->flags |= CMD_NOSCRIPT; break;
				case 'R': c->flags |= CMD_RANDOM; break;
				case 'S': c->flags |= CMD_SORT_FOR_SCRIPT; break;
				case 'l': c->flags |= CMD_LOADING; break;
				case 't': c->flags |= CMD_STALE; break;
				case 'M': c->flags |= CMD_SKIP_MONITOR; break;
				case 'k': c->flags |= CMD_ASKING; break;
				case 'F': c->flags |= CMD_FAST; break;
				default: serverPanic("Unsupported command flag"); break;
			}
			f++;
		}

		retval1 = dictAdd(server.commands, sdsnew(c->name), c);
		/* Populate an additional dictionary that will be unaffected
		 * by rename-command statements in redis.conf. */
		retval2 = dictAdd(server.orig_commands, sdsnew(c->name), c);
		serverAssert(retval1 == DICT_OK && retval2 == DICT_OK);
	}
}


/* ========================== Redis OP Array API ============================ */
// 初始化一个Redis操作数组
void redisOpArrayInit(redisOpArray *oa) {
    oa->ops = NULL;
    oa->numops = 0;
}


/* ====================== Commands lookup and execution ===================== */

// 根据name查找返回对应的命令
struct redisCommand *lookupCommand(sds name) {
    return dictFetchValue(server.commands, name);
}

// call()是Redis执行命令的核心
// flags 可以指定一下值：
/*
    CMD_CALL_NONE：没有指定flags
    CMD_CALL_SLOWLOG：检查命令的执行速度，如果需要记录在慢查询日志中
    CMD_CALL_STATS：记录命令的统计信息
    CMD_CALL_PROPAGATE_AOF：如果client设置了强制传播的标志或修改了数据集，则将命令追加到AOF文件中
    CMD_CALL_PROPAGATE_REPL：如果client设置了强制传播的标志或修改了数据集，则将命令发送给从节点服务器中
    CMD_CALL_PROPAGATE：如果client设置了强制传播的标志或修改了数据集，则将命令发送给从节点服务器或追加到AOF中
    CMD_CALL_FULL：包含以上所有的含义
*/
// 实际的传播行为依赖于client的flags标志，特别地
/*
    1、如果client的flags指定了CLIENT_FORCE_AOF or CLIENT_FORCE_REPL并且假定命令也制定了CMD_CALL_PROPAGATE_AOF/REPL，那么即使数据集没有被修改，也会传播命令。
    2、如果client的flags指定了CLIENT_PREVENT_REPL_PROP or CLIENT_PREVENT_AOF_PROP，
    即使数据集被命令修改，也不会传播到AOF文件或从节点服务器中。
*/
// 不管client的flags如何指定，只要CMD_CALL_PROPAGATE_AOF or CMD_CALL_PROPAGATE_REPL没有被指定，传播操作将不会发生
// client的flags被修改的接口如下：
void call(client *c, int flags) {
    long long dirty, start, duration;
    int client_old_flags = c->flags;    //备份client的flags

    /* Sent the command to clients in MONITOR mode, only if the commands are
     * not generated from reading an AOF. */
    // 将命令发送给 MONITOR
    //if (listLength(server.monitors) &&
    //    !server.loading &&
    //    !(c->cmd->flags & (CMD_SKIP_MONITOR|CMD_ADMIN)))
    //{
    //    replicationFeedMonitors(c,server.monitors,c->db->id,c->argv,c->argc);
    //}

    /* Initialization: clear the flags that must be set by the command on
     * demand, and initialize the array for additional commands propagation. */
    // 清除一些需要按照命令需求设置的标志，以防干扰
    c->flags &= ~(CLIENT_FORCE_AOF|CLIENT_FORCE_REPL|CLIENT_PREVENT_PROP);
    // 初始化Redis操作数组，用来追加命令的传播
    redisOpArrayInit(&server.also_propagate);

    /* Call the command. */
    // 备份脏键数
    dirty = server.dirty;
    // 获取执行命令的开始时间
    start = ustime();
    // 执行命令
    c->cmd->proc(c);
    // 命令的执行时间
    duration = ustime()-start;
    // 命令修改的键的个数
    dirty = server.dirty-dirty;
    if (dirty < 0) dirty = 0;

    /* When EVAL is called loading the AOF we don't want commands called
     * from Lua to go into the slowlog or to populate statistics. */
    // 当执行 EVAL 命令时正在加载AOF，而且不希望Lua调用的命令进入slowlog或填充统计信息
    if (server.loading && c->flags & CLIENT_LUA)
        flags &= ~(CMD_CALL_SLOWLOG | CMD_CALL_STATS);  //取消慢查询和记录统计信息的标志

    /* If the caller is Lua, we want to force the EVAL caller to propagate
     * the script if the command flag or client flag are forcing the
     * propagation. */
    // 如果函数调用者是Lua脚本，且命令的flags或客户端的flags指定了强制传播，我们要强制EVAL调用者传播脚本
    if (c->flags & CLIENT_LUA && server.lua_caller) {
        // 如果指定了强制将命令传播到从节点
        if (c->flags & CLIENT_FORCE_REPL)
            server.lua_caller->flags |= CLIENT_FORCE_REPL;  //强制执行lua脚本的client要传播命令到从节点
        // 如果指定了强制将节点传播到AOF中
        if (c->flags & CLIENT_FORCE_AOF)
            server.lua_caller->flags |= CLIENT_FORCE_AOF;   //强制执行lua脚本的client要传播命令到AOF文件
    }

    ///* Log the command into the Slow log if needed, and populate the
    // * per-command statistics that we show in INFO commandstats. */
    //// 命令的flags指定了慢查询标志，要将总的统计信息推入慢查询日志中
    //if (flags & CMD_CALL_SLOWLOG && c->cmd->proc != execCommand) {
    //    char *latency_event = (c->cmd->flags & CMD_FAST) ?
    //                          "fast-command" : "command";
    //    // 记录将延迟事件和延迟时间关联到延迟诊断的字典中
    //    latencyAddSampleIfNeeded(latency_event,duration/1000);
    //    // 将总的统计信息推入慢查询日志中
    //    slowlogPushEntryIfNeeded(c->argv,c->argc,duration);
    //}
    //// 命令的flags指定了CMD_CALL_STATS，更新命令的统计信息
    //if (flags & CMD_CALL_STATS) {
    //    c->lastcmd->microseconds += duration;
    //    c->lastcmd->calls++;
    //}

    ///* Propagate the command into the AOF and replication link */
    //// 如果client设置了强制传播的标志或修改了数据集，则将命令发送给从节点服务器或追加到AOF中
    //if (flags & CMD_CALL_PROPAGATE &&
    //    (c->flags & CLIENT_PREVENT_PROP) != CLIENT_PREVENT_PROP)
    //{
    //    // 保存传播的标志，初始化为空
    //    int propagate_flags = PROPAGATE_NONE;

    //    /* Check if the command operated changes in the data set. If so
    //     * set for replication / AOF propagation. */
    //    // 如果命令修改了数据库中的键，则要传播到AOF和从节点中
    //    if (dirty) propagate_flags |= (PROPAGATE_AOF|PROPAGATE_REPL);

    //    /* If the client forced AOF / replication of the command, set
    //     * the flags regardless of the command effects on the data set. */
    //    // 如果client设置了强制AOF和复制的标志，则设置传播的标志
    //    if (c->flags & CLIENT_FORCE_REPL) propagate_flags |= PROPAGATE_REPL;
    //    if (c->flags & CLIENT_FORCE_AOF) propagate_flags |= PROPAGATE_AOF;

    //    /* However prevent AOF / replication propagation if the command
    //     * implementatino called preventCommandPropagation() or similar,
    //     * or if we don't have the call() flags to do so. */
    //    // 如果client的flags设置了CLIENT_PREVENT_REPL/AOF_PROP，表示阻止命令的传播到从节点或AOF，则取消传播对应标志
    //    if (c->flags & CLIENT_PREVENT_REPL_PROP ||
    //        !(flags & CMD_CALL_PROPAGATE_REPL))
    //            propagate_flags &= ~PROPAGATE_REPL;
    //    if (c->flags & CLIENT_PREVENT_AOF_PROP ||
    //        !(flags & CMD_CALL_PROPAGATE_AOF))
    //            propagate_flags &= ~PROPAGATE_AOF;

    //    /* Call propagate() only if at least one of AOF / replication
    //     * propagation is needed. */
    //    // 如果至少设置了一种传播，则执行相应传播命令操作
    //    if (propagate_flags != PROPAGATE_NONE)
    //        propagate(c->cmd,c->db->id,c->argv,c->argc,propagate_flags);
    //}

    /* Restore the old replication flags, since call() can be executed
     * recursively. */
    // 清除一些需要按照命令需求设置的标志，以防干扰
    c->flags &= ~(CLIENT_FORCE_AOF|CLIENT_FORCE_REPL|CLIENT_PREVENT_PROP);
    // 恢复client原始的flags
    c->flags |= client_old_flags &
        (CLIENT_FORCE_AOF|CLIENT_FORCE_REPL|CLIENT_PREVENT_PROP);

    //// 传播追加在Redis操作数组中的命令
    //if (server.also_propagate.numops) {
    //    int j;
    //    redisOp *rop;
    //    // 如果命令的flags设置传播的标志
    //    if (flags & CMD_CALL_PROPAGATE) {
    //        // 遍历所有的命令
    //        for (j = 0; j < server.also_propagate.numops; j++) {
    //            rop = &server.also_propagate.ops[j];
    //            int target = rop->target;
    //            /* Whatever the command wish is, we honor the call() flags. */
    //            // 执行相应传播命令操作
    //            if (!(flags&CMD_CALL_PROPAGATE_AOF)) target &= ~PROPAGATE_AOF;
    //            if (!(flags&CMD_CALL_PROPAGATE_REPL)) target &= ~PROPAGATE_REPL;
    //            if (target)
    //                propagate(rop->cmd,rop->dbid,rop->argv,rop->argc,target);
    //        }
    //    }
    //    // 释放Redis操作数组
    //    redisOpArrayFree(&server.also_propagate);
    //}
    // 命令执行的次数加1
    server.stat_numcommands++;
}//end of call()

// 如果函数被调用时，服务器已经读入了一整套命令参数，
// 保存在参数列表中，processCommand()执行命令或者准备从client读取
// 如果client没有被关闭则返回C_OK，调用者可以继续执行其他的操作，
// 否则返回C_ERR，表示client被销毁
int processCommand(client *c) {
    /* The QUIT command is handled separately. Normal command procs will
     * go through checking for replication and QUIT will cause trouble
     * when FORCE_REPLICATION is enabled and would be implemented in
     * a regular command proc. */
    // 如果是 quit 命令，则单独处理
    if (!strcasecmp(c->argv[0]->ptr,"quit")) {
        addReply(c,shared.ok);
        c->flags |= CLIENT_CLOSE_AFTER_REPLY;   //设置client的状态为回复后立即关闭，返回C_ERR
        return C_ERR;
    }

    /* Now lookup the command and check ASAP about trivial error conditions
     * such as wrong arity, bad command name and so forth. */
    // 从数据库的字典中查找该命令
    c->cmd = c->lastcmd = lookupCommand(c->argv[0]->ptr);
    // 不存在的命令
    if (!c->cmd) {
        flagTransaction(c); //如果是事务状态的命令，则设置事务为失败
        addReplyErrorFormat(c,"unknown command '%s'",
            (char*)c->argv[0]->ptr);
        return C_OK;
    // 参数数量不匹配
    } else if ((c->cmd->arity > 0 && c->cmd->arity != c->argc) ||
               (c->argc < -c->cmd->arity)) {
        flagTransaction(c); //如果是事务状态的命令，则设置事务为失败
        addReplyErrorFormat(c,"wrong number of arguments for '%s' command",
            c->cmd->name);
        return C_OK;
    }

    ///* Check if the user is authenticated */
    //// 如果服务器设置了密码，但是没有认证成功
    //if (server.requirepass && !c->authenticated && c->cmd->proc != authCommand)
    //{
    //    flagTransaction(c); //如果是事务状态的命令，则设置事务为失败
    //    addReply(c,shared.noautherr);
    //    return C_OK;
    //}

    /* If cluster is enabled perform the cluster redirection here.
     * However we don't perform the redirection if:
     * 1) The sender of this command is our master.
     * 2) The command has no key arguments. */
    // 如果开启了集群模式，则执行集群的重定向操作，下面的两种情况例外：
    /*
        1. 命令的发送是主节点服务器
        2. 命令没有key
    */
    if (server.cluster_enabled &&
        !(c->flags & CLIENT_MASTER) &&
        !(c->flags & CLIENT_LUA &&
          server.lua_caller->flags & CLIENT_MASTER) &&
        !(c->cmd->getkeys_proc == NULL && c->cmd->firstkey == 0 &&
          c->cmd->proc != execCommand))
    {
        //int hashslot;
        //int error_code;
        //// 从集群中返回一个能够执行命令的节点
        //clusterNode *n = getNodeByQuery(c,c->cmd,c->argv,c->argc,
        //                                &hashslot,&error_code);
        //// 返回的节点不合格
        //if (n == NULL || n != server.cluster->myself) {
        //    // 如果是执行事务的命令，则取消事务
        //    if (c->cmd->proc == execCommand) {
        //        discardTransaction(c);
        //    } else {
        //        // 将事务状态设置为失败
        //        flagTransaction(c);
        //    }
        //    // 执行client的重定向操作
        //    clusterRedirectClient(c,n,hashslot,error_code);
        //    return C_OK;
        //}
    }

    /* Handle the maxmemory directive.
     *
     * First we try to free some memory if possible (if there are volatile
     * keys in the dataset). If there are not the only thing we can do
     * is returning an error. */
    // 如果服务器有最大内存的限制
    if (server.maxmemory) {
        // 按需释放一部分内存
        int retval = freeMemoryIfNeeded();
        /* freeMemoryIfNeeded may flush slave output buffers. This may result
         * into a slave, that may be the active client, to be freed. */
        // freeMemoryIfNeeded()函数之后需要冲洗从节点的输出缓冲区，这可能导致被释放的从节点是一个活跃的client
        // 如果当前的client被释放，返回C_ERR
        if (server.current_client == NULL) return C_ERR;

        /* It was impossible to free enough memory, and the command the client
         * is trying to execute is denied during OOM conditions? Error. */
        // 如果命令会耗费大量的内存但是释放内存失败
        if ((c->cmd->flags & CMD_DENYOOM) && retval == C_ERR) {
            // 将事务状态设置为失败
            flagTransaction(c);
            addReply(c, shared.oomerr);
            return C_OK;
        }
    }

    /* Don't accept write commands if there are problems persisting on disk
     * and if this is a master instance. */
    // 如果 BGSAVE 命令执行错误而且服务器是一个主节点，那么不接受写命令
    if (((server.stop_writes_on_bgsave_err &&
          server.saveparamslen > 0 &&
          server.lastbgsave_status == C_ERR) ||
          server.aof_last_write_status == C_ERR) &&
        server.masterhost == NULL &&
        (c->cmd->flags & CMD_WRITE ||
         c->cmd->proc == pingCommand))
    {
        //// 将事务状态设置为失败
        //flagTransaction(c);
        //// 如果上一次执行AOF成功回复BGSAVE错误回复
        //if (server.aof_last_write_status == C_OK)
        //    addReply(c, shared.bgsaveerr);
        //else
        //    addReplySds(c,
        //        sdscatprintf(sdsempty(),
        //        "-MISCONF Errors writing to the AOF file: %s\r\n",
        //        strerror(server.aof_last_write_errno)));
        //return C_OK;
    }

    /* Don't accept write commands if there are not enough good slaves and
     * user configured the min-slaves-to-write option. */
    // 如果没有足够的良好的从节点而且用户配置了 min-slaves-to-write，那么不接受写命令
    if (server.masterhost == NULL &&
        server.repl_min_slaves_to_write &&
        server.repl_min_slaves_max_lag &&
        c->cmd->flags & CMD_WRITE &&
        server.repl_good_slaves_count < server.repl_min_slaves_to_write)
    {
        // 将事务状态设置为失败
        flagTransaction(c);
        addReply(c, shared.noreplicaserr);
        return C_OK;
    }

    /* Don't accept write commands if this is a read only slave. But
     * accept write commands if this is our master. */
    // 如果这是一个只读的从节点服务器，则不接受写命令
    if (server.masterhost && server.repl_slave_ro &&
        !(c->flags & CLIENT_MASTER) &&
        c->cmd->flags & CMD_WRITE)
    {
        addReply(c, shared.roslaveerr);
        return C_OK;
    }

    ///* Only allow SUBSCRIBE and UNSUBSCRIBE in the context of Pub/Sub */
    //// 如果处于发布订阅模式，但是执行的不是发布订阅命令，返回
    //if (c->flags & CLIENT_PUBSUB &&
    //    c->cmd->proc != pingCommand &&
    //    c->cmd->proc != subscribeCommand &&
    //    c->cmd->proc != unsubscribeCommand &&
    //    c->cmd->proc != psubscribeCommand &&
    //    c->cmd->proc != punsubscribeCommand) {
    //    addReplyError(c,"only (P)SUBSCRIBE / (P)UNSUBSCRIBE / PING / QUIT allowed in this context");
    //    return C_OK;
    //}

    /* Only allow INFO and SLAVEOF when slave-serve-stale-data is no and
     * we are a slave with a broken link with master. */
    // 如果是从节点且和主节点断开了连接，不允许从服务器带有过期数据，返回
    if (server.masterhost && server.repl_state != REPL_STATE_CONNECTED &&
        server.repl_serve_stale_data == 0 &&
        !(c->cmd->flags & CMD_STALE))
    {
        flagTransaction(c);
        addReply(c, shared.masterdownerr);
        return C_OK;
    }

    /* Loading DB? Return an error if the command has not the
     * CMD_LOADING flag. */
    // 如果服务器处于载入状态，如果命令不是CMD_LOADING标识，则不执行，返回
    if (server.loading && !(c->cmd->flags & CMD_LOADING)) {
        addReply(c, shared.loadingerr);
        return C_OK;
    }

    ///* Lua script too slow? Only allow a limited number of commands. */
    //// 如果lua脚本超时，限制执行一部分命令，如shutdown、scriptCommand
    //if (server.lua_timedout &&
    //      c->cmd->proc != authCommand &&
    //      c->cmd->proc != replconfCommand &&
    //    !(c->cmd->proc == shutdownCommand &&
    //      c->argc == 2 &&
    //      tolower(((char*)c->argv[1]->ptr)[0]) == 'n') &&
    //    !(c->cmd->proc == scriptCommand &&
    //      c->argc == 2 &&
    //      tolower(((char*)c->argv[1]->ptr)[0]) == 'k'))
    //{
    //    flagTransaction(c);
    //    addReply(c, shared.slowscripterr);
    //    return C_OK;
    //}

    /* Exec the command */
    // 执行命令
    // client处于事务环境中，但是执行命令不是exec、discard、multi和watch
    //if (c->flags & CLIENT_MULTI &&
    //    c->cmd->proc != execCommand && c->cmd->proc != discardCommand &&
    //    c->cmd->proc != multiCommand && c->cmd->proc != watchCommand)
    //{
    //    // 除了上述的四个命令，其他的命令添加到事务队列中
    //    queueMultiCommand(c);
    //    addReply(c,shared.queued);
    //// 执行普通的命令
    //} else {
        call(c,CMD_CALL_FULL);
        // 保存写全局的复制偏移量
        c->woff = server.master_repl_offset;
        //// 如果因为BLPOP而阻塞的命令已经准备好，则处理client的阻塞状态
        //if (listLength(server.ready_keys))
        //    handleClientsBlockedOnLists();
    //}
    return C_OK;
}//end of processCommand()

/*====================== Hash table type implementation  ==================== */

// 计算是否需要进行字典的resize操作
int htNeedsResize(dict *dict) {
	long long size, used;

	// 获取字典已使用的大小
	size = dictSlots(dict);
	// 获取字典总共的大小
	used = dictSize(dict);
	// 如果size超过字典的哈希表初始大小4，且使用率小于10%
	return (size > DICT_HT_INITIAL_SIZE &&
			(used*100/size < HASHTABLE_MIN_FILL));
}


// 创建并返回一个新的回收池
struct evictionPoolEntry *evictionPoolAlloc(void) {
	struct evictionPoolEntry *ep;
	int j;

	// 分配回收池大小
	ep = zmalloc(sizeof(*ep)*MAXMEMORY_EVICTION_POOL_SIZE);
	// 初始化回收池
	for (j = 0; j < MAXMEMORY_EVICTION_POOL_SIZE; j++) {
		ep[j].idle = 0;
		ep[j].key = NULL;
	}
	return ep;
}//end of evictionPoolEntry()

// 按需释放内存空间
int freeMemoryIfNeeded(void) {
	//TODO
	return C_OK;
}

/* =================================== Main! ================================ */
#ifdef __linux__
//  /proc/sys/vm/overcommit_memory文件含义
// 该文件指定了内核针对内存分配的策略，其值可以是0、1、2。
// 0，表示内核将检查是否有足够的可用内存供应用进程使用；如果有足够的可用内存，内存申请允许；否则，内存申请失败，并把错误返回给应用进程。
// 1，表示内核允许分配所有的物理内存，而不管当前的内存状态如何。
// 2，表示内核允许分配超过所有物理内存和交换空间总和的内存
int linuxOvercommitMemoryValue(void) {
	FILE *fp = fopen("/proc/sys/vm/overcommit_memory","r");
	char buf[64];

	if (!fp) return -1;
	if (fgets(buf,64,fp) == NULL) {
		fclose(fp);
		return -1;
	}
	fclose(fp);

	return atoi(buf);
}

// 系统内存的日志
void linuxMemoryWarnings(void) {
	// 如果有足够的可用内存，内存申请允许；否则，内存申请失败，并把错误返回给应用进程。
	if (linuxOvercommitMemoryValue() == 0) {
		serverLog(LL_WARNING,"WARNING overcommit_memory is set to 0! Background save may fail under low memory condition. To fix this issue add 'vm.overcommit_memory = 1' to /etc/sysctl.conf and then reboot or run the command 'sysctl vm.overcommit_memory=1' for this to take effect.");
	}
	// 如果Linux支持THP内存，打印日志
	if (THPIsEnabled()) {
		serverLog(LL_WARNING,"WARNING you have Transparent Huge Pages (THP) support enabled in your kernel. This will create latency and memory usage issues with Redis. To fix this issue run the command 'echo never > /sys/kernel/mm/transparent_hugepage/enabled' as root, and add it to your /etc/rc.local in order to retain the setting after a reboot. Redis must be restarted after THP is disabled.");
	}
}
#endif /* __linux__ */




// 创建pidfile
void createPidFile(void) {
	/* If pidfile requested, but no pidfile defined, use
	 * default pidfile path */
	// 如果没有设置pidfile，则设置一个默认路径的pidfile
	if (!server.pidfile) server.pidfile = zstrdup(CONFIG_DEFAULT_PID_FILE);

	/* Try to write the pid file in a best-effort way. */
	// 将子进程pid写到文件中
	FILE *fp = fopen(server.pidfile,"w");
	if (fp) {
		fprintf(fp,"%d\n",(int)getpid());
		fclose(fp);
	}
}



void daemonize(void) {
	int fd;

	if (fork() != 0) exit(0); /* parent exits */
	setsid(); /* create a new session */

	/* Every output goes to /dev/null. If Redis is daemonized but
	 * the 'logfile' is set to 'stdout' in the configuration file
	 * it will not log at all. */
	if ((fd = open("/dev/null", O_RDWR, 0)) != -1) {
		dup2(fd, STDIN_FILENO);
		dup2(fd, STDOUT_FILENO);
		dup2(fd, STDERR_FILENO);
		if (fd > STDERR_FILENO) close(fd);
	}
}


/* Returns 1 if there is --sentinel among the arguments or if
 *  * argv[0] is exactly "redis-sentinel". */
int checkForSentinelMode(int argc, char **argv) {
	int j;

	if (strstr(argv[0],"redis-sentinel") != NULL) return 1;
	for (j = 1; j < argc; j++)
		if (!strcmp(argv[j],"--sentinel")) return 1;
	return 0;
}


void redisSetProcTitle(char *title) {
#ifdef USE_SETPROCTITLE
	char *server_mode = "";
	if (server.cluster_enabled) server_mode = " [cluster]";
	else if (server.sentinel_mode) server_mode = " [sentinel]";

	setproctitle("%s %s:%d%s",
			title,
			server.bindaddr_count ? server.bindaddr[0] : "*",
			server.port,
			server_mode);
#else
	UNUSED(title);
#endif
}

void redisOutOfMemoryHandler(size_t allocation_size) {
	serverLog(LL_WARNING,"Out Of Memory allocating %zu bytes!",
			allocation_size);
	serverPanic("Redis aborting for OUT OF MEMORY");
}


int redisIsSupervised(int mode) {
	return 0;
}


/*========================= ybx ================================================= */

void init()
{
	initServerConfig();
	initServer();
}

void print_obj_encoding(robj * set_obj)
{
	if(set_obj->encoding ==OBJ_ENCODING_RAW){
		printf("对象编码类型为RAW\n");
	}else if(set_obj->encoding ==OBJ_ENCODING_INT){
		printf("对象编码类型为INT\n");
	}else if(set_obj->encoding ==OBJ_ENCODING_HT){
		printf("对象编码类型为HT\n");
	}else if(set_obj->encoding ==OBJ_ENCODING_ZIPMAP){
		printf("对象编码类型为ZIPMAP\n");
	}else if(set_obj->encoding ==OBJ_ENCODING_LINKEDLIST){
		printf("对象编码类型为LINKEDLIST\n");
	}else if(set_obj->encoding ==OBJ_ENCODING_ZIPLIST){
		printf("对象编码类型为ZIPLIST\n");
	}else if (set_obj->encoding == OBJ_ENCODING_INTSET) {
		printf("对象编码类型为INTSET\n");
	}else if(set_obj->encoding ==OBJ_ENCODING_SKIPLIST){
		printf("对象编码类型为SKIPLIST\n");
	}else if(set_obj->encoding ==OBJ_ENCODING_EMBSTR){
		printf("对象编码类型为EMBSTR\n");
	}else if(set_obj->encoding ==OBJ_ENCODING_QUICKLIST){
		printf("对象编码类型为QUICKLIST\n");
	}else{
		printf("未知对象编码类型:%dn",set_obj->encoding);
	}
}
