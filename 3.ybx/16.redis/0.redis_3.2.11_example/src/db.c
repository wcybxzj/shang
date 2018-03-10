#include "server.h"
#include "cluster.h"

#include <signal.h>
#include <ctype.h>

void slotToKeyAdd(robj *key);
void slotToKeyDel(robj *key);
void slotToKeyFlush(void);

/*-----------------------------------------------------------------------------
 * C-level DB API
 *----------------------------------------------------------------------------*/
/* Low level key lookup API, not actually called directly from commands
 * implementations that should instead rely on lookupKeyRead(),
 * lookupKeyWrite() and lookupKeyReadWithFlags(). */
// 该函数被lookupKeyRead()和lookupKeyWrite()和lookupKeyReadWithFlags()调用
// 从数据库db中取出key的值对象，如果存在返回该对象，否则返回NULL
// 返回key对象的值对象
robj *lookupKey(redisDb *db, robj *key, int flags) {
    // 在数据库中查找key对象，返回保存该key的节点地址
    dictEntry *de = dictFind(db->dict,key->ptr);
    if (de) {   //如果找到
        robj *val = dictGetVal(de); //取出键对应的值对象

        /* Update the access time for the ageing algorithm.
         * Don't do it if we have a saving child, as this will trigger
         * a copy on write madness. */
        // 更新键的使用时间
        if (server.rdb_child_pid == -1 &&
            server.aof_child_pid == -1 &&
            !(flags & LOOKUP_NOTOUCH))
        {
            val->lru = LRU_CLOCK();
        }
        return val; //返回值对象
    } else {
        return NULL;
    }
}

// 以读操作取出key的值对象，没找到返回NULL
// 调用该函数的副作用如下：
// 1.如果一个键的到达过期时间TTL，该键被设置为过期的
// 2.键的使用时间信息被更新
// 3.全局键 hits/misses 状态被更新
// 注意：如果键在逻辑上已经过期但是仍然存在，函数返回NULL
robj *lookupKeyReadWithFlags(redisDb *db, robj *key, int flags) {
    robj *val;

    // 如果键已经过期且被删除
    if (expireIfNeeded(db,key) == 1) {
        /* Key expired. If we are in the context of a master, expireIfNeeded()
         * returns 0 only when the key does not exist at all, so it's save
         * to return NULL ASAP. */
        // 键已过期，如果是主节点环境，表示key已经绝对被删除，如果是从节点，
        if (server.masterhost == NULL) return NULL;

        // 如果我们在从节点环境， expireIfNeeded()函数不会删除过期的键，它返回的仅仅是键是否被删除的逻辑值
        // 过期的键由主节点负责，为了保证主从节点数据的一致
        if (server.current_client &&
            server.current_client != server.master &&
            server.current_client->cmd &&
            server.current_client->cmd->flags & CMD_READONLY)
        {
            return NULL;
        }
    }
    // 键没有过期，则返回键的值对象
    val = lookupKey(db,key,flags);
    // 更新 是否命中 的信息
    if (val == NULL)
        server.stat_keyspace_misses++;
    else
        server.stat_keyspace_hits++;
    return val;
}

// 以读操作取出key的值对象，会更新是否命中的信息
robj *lookupKeyRead(redisDb *db, robj *key) {
    return lookupKeyReadWithFlags(db,key,LOOKUP_NONE);
}

// 以写操作取出key的值对象，不更新是否命中的信息
robj *lookupKeyWrite(redisDb *db, robj *key) {
    expireIfNeeded(db,key);
    return lookupKey(db,key,LOOKUP_NONE);
}

robj *lookupKeyReadOrReply(client *c, robj *key, robj *reply) {
    robj *o = lookupKeyRead(c->db, key);
    if (!o) addReply(c,reply);
    return o;
}

// 讲key-val键值对添加到数据库中，该函数的调用者负责增加key-val的引用计数
void dbAdd(redisDb *db, robj *key, robj *val) {
    sds copy = sdsdup(key->ptr);    //复制key字符串
    int retval = dictAdd(db->dict, copy, val);  //将key-val添加到键值对字典

    serverAssertWithInfo(NULL,key,retval == DICT_OK);
    // 如果值对象是列表类型，有阻塞的命令，因此将key加入ready_keys字典中
    if (val->type == OBJ_LIST) signalListAsReady(db, key);
    // 如果开启了集群模式，则讲key添加到槽中
    if (server.cluster_enabled) slotToKeyAdd(key);
}

// 用一个新的val重写已经存在key，该函数的调用者负责增加key-val的引用计数
// 该函数不修改该key的过期时间，如果key不存在，则程序终止
void dbOverwrite(redisDb *db, robj *key, robj *val) {
	dictEntry *de = dictFind(db->dict,key->ptr);    //找到保存key的节点地址
	serverAssertWithInfo(NULL,key,de != NULL);      //确保key被找到
	dictReplace(db->dict, key->ptr, val);           //重写val
}

long long emptyDb(void(callback)(void*)) {
    int j;
    long long removed = 0;

    for (j = 0; j < server.dbnum; j++) {
        removed += dictSize(server.db[j].dict);
        dictEmpty(server.db[j].dict,callback);
        dictEmpty(server.db[j].expires,callback);
    }
    if (server.cluster_enabled) slotToKeyFlush();
    return removed;
}


int selectDb(client *c, int id) {
    if (id < 0 || id >= server.dbnum)
        return C_ERR;
    c->db = &server.db[id];
    return C_OK;
}

/*-----------------------------------------------------------------------------
 * Hooks for key space changes.
 *
 * Every time a key in the database is modified the function
 * signalModifiedKey() is called.
 *
 * Every time a DB is flushed the function signalFlushDb() is called.
 *----------------------------------------------------------------------------*/

void signalModifiedKey(redisDb *db, robj *key) {
    //touchWatchedKey(db,key);
}

void signalFlushedDb(int dbid) {
    //touchWatchedKeysOnFlush(dbid);
}



void delCommand(client *c) {
    int deleted = 0, j;

    for (j = 1; j < c->argc; j++) {
        expireIfNeeded(c->db,c->argv[j]);
        if (dbDelete(c->db,c->argv[j])) {
            signalModifiedKey(c->db,c->argv[j]);
            notifyKeyspaceEvent(NOTIFY_GENERIC,
                "del",c->argv[j],c->db->id);
            server.dirty++;
            deleted++;
        }
    }
    addReplyLongLong(c,deleted);
}


void selectCommand(client *c) {
    long id;

    if (getLongFromObjectOrReply(c, c->argv[1], &id,
        "invalid DB index") != C_OK)
        return;

    if (server.cluster_enabled && id != 0) {
        addReplyError(c,"SELECT is not allowed in cluster mode");
        return;
    }
    if (selectDb(c,id) == C_ERR) {
        addReplyError(c,"invalid DB index");
    } else {
        addReply(c,shared.ok);
    }
}

void keysCommand(client *c) {
    dictIterator *di;
    dictEntry *de;
    sds pattern = c->argv[1]->ptr;
    int plen = sdslen(pattern), allkeys;
    unsigned long numkeys = 0;
    void *replylen = addDeferredMultiBulkLength(c);

    di = dictGetSafeIterator(c->db->dict);
    allkeys = (pattern[0] == '*' && pattern[1] == '\0');
    while((de = dictNext(di)) != NULL) {
        sds key = dictGetKey(de);
        robj *keyobj;

        if (allkeys || stringmatchlen(pattern,plen,key,sdslen(key),0)) {
            keyobj = createStringObject(key,sdslen(key));
            if (expireIfNeeded(c->db,keyobj) == 0) {
                addReplyBulk(c,keyobj);
                numkeys++;
            }
            decrRefCount(keyobj);
        }
    }
    dictReleaseIterator(di);
    setDeferredMultiBulkLength(c,replylen,numkeys);
}

// 高级的设置key，无论key是否存在，都将val与其关联
// 1.value对象的引用计数被增加
// 2.监控key的客户端收到键被修改的通知
// 3.键的过期时间被设置为永久
void setKey(redisDb *db, robj *key, robj *val) {
	// 如果key不存在
	if (lookupKeyWrite(db,key) == NULL) {
		dbAdd(db,key,val);  //讲key-val添加到db中
	} else {    //key存在
		dbOverwrite(db,key,val);    //用val讲key的原值覆盖
	}
	incrRefCount(val);          //val引用计数加1
	removeExpire(db,key);       //移除key的过期时间
	signalModifiedKey(db,key);  //发送键被修改的信号
}

// 删除一个键值对以及键的过期时间，返回1表示删除成功
int dbDelete(redisDb *db, robj *key) {
    // 过期字典中有键，那么将key对象从过期字典中删除
    if (dictSize(db->expires) > 0) dictDelete(db->expires,key->ptr);
    // 将key-value从键值对字典中删除
    if (dictDelete(db->dict,key->ptr) == DICT_OK) {
        // 如果开启了集群模式，那么从槽中删除给定的键
        if (server.cluster_enabled) slotToKeyDel(key);
        return 1;
    } else {
        return 0;
    }
}

/*-----------------------------------------------------------------------------
 * Expires API
 *----------------------------------------------------------------------------*/
// 移除key的过期时间，成功返回1
int removeExpire(redisDb *db, robj *key) {
    /* An expire may only be removed if there is a corresponding entry in the
     * main dict. Otherwise, the key will never be freed. */
    // key存在于键值对字典中
    serverAssertWithInfo(NULL,key,dictFind(db->dict,key->ptr) != NULL);
    // 从过期字典中删除该键
    return dictDelete(db->expires,key->ptr) == DICT_OK;
}

// 设置过期时间
void setExpire(redisDb *db, robj *key, long long when) {
    dictEntry *kde, *de;
    /* Reuse the sds from the main dict in the expire dict */
    kde = dictFind(db->dict,key->ptr);
    serverAssertWithInfo(NULL,key,kde != NULL);
    de = dictReplaceRaw(db->expires,dictGetKey(kde));
    dictSetSignedIntegerVal(de,when);
}

// 返回一个键的过期时间，如果该键没有设定过期时间，则返回-1
long long getExpire(redisDb *db, robj *key) {
    dictEntry *de;

    /* No expire? return ASAP */
    // 如果过期字典为空，或者过期字典中找不到指定的key，立即返回
    if (dictSize(db->expires) == 0 ||
       (de = dictFind(db->expires,key->ptr)) == NULL) return -1;

    /* The entry was found in the expire dict, this means it should also
     * be present in the main dict (safety check). */
    // 保存当前key的节点de不为空，说明该key设置了过期时间
    // 还要保证该key在键值对字典中存在
    serverAssertWithInfo(NULL,key,dictFind(db->dict,key->ptr) != NULL);
    // 将de节点的有符号整数据返回，也就是过期时间
    return dictGetSignedIntegerVal(de);
}

// 将过期时间传播到从节点和AOF文件
// 当一个键在主节点中过期时，主节点会发送del命令给从节点和AOF文件
void propagateExpire(redisDb *db, robj *key) {
	//方便调试先空着
}

// 检查键是否过期，如果过期，从数据库中删除
// 返回0表示没有过期或没有过期时间，返回1 表示键被删除
int expireIfNeeded(redisDb *db, robj *key) {
    //得到过期时间，单位毫秒
    mstime_t when = getExpire(db,key);
    mstime_t now;

    // 没有过期时间，直接返回
    if (when < 0) return 0; /* No expire for this key */

    /* Don't expire anything while loading. It will be done later. */
    // 服务器正在载入，那么不进行过期检查
    if (server.loading) return 0;

    // 返回一个Unix时间，单位毫秒
    now = server.lua_caller ? server.lua_time_start : mstime();

    // 如果服务器正在进行主从节点的复制，从节点的过期键应该被 主节点发送同步删除的操作 删除，而自己不主动删除
    // 从节点只返回正确的逻辑信息，0表示key仍然没有过期，1表示key过期。
    if (server.masterhost != NULL) return now > when;

    /* Return when this key has not expired */
    // 当键还没有过期时，直接返回0
    if (now <= when) return 0;

    /* Delete the key */
    // 键已经过期，删除键
    server.stat_expiredkeys++;              //过期键的数量加1
    propagateExpire(db,key);                //将过期键key传播给AOF文件和从节点
    notifyKeyspaceEvent(NOTIFY_EXPIRED,     //发送"expired"事件通知
        "expired",key,db->id);
    return dbDelete(db,key);                //从数据库中删除key
}



/*-----------------------------------------------------------------------------
 * Expires Commands
 *----------------------------------------------------------------------------*/

/* This is the generic command implementation for EXPIRE, PEXPIRE, EXPIREAT
 * and PEXPIREAT. Because the commad second argument may be relative or absolute
 * the "basetime" argument is used to signal what the base time is (either 0
 * for *AT variants of the command, or the current time for relative expires).
 *
 * unit is either UNIT_SECONDS or UNIT_MILLISECONDS, and is only used for
 * the argv[2] parameter. The basetime is always specified in milliseconds. */
void expireGenericCommand(client *c, long long basetime, int unit) {
    robj *key = c->argv[1], *param = c->argv[2];
    long long when; /* unix time in milliseconds when the key will expire. */

    if (getLongLongFromObjectOrReply(c, param, &when, NULL) != C_OK)
        return;

    if (unit == UNIT_SECONDS) when *= 1000;
    when += basetime;

    /* No key, return zero. */
    if (lookupKeyWrite(c->db,key) == NULL) {
        addReply(c,shared.czero);
        return;
    }

    /* EXPIRE with negative TTL, or EXPIREAT with a timestamp into the past
     * should never be executed as a DEL when load the AOF or in the context
     * of a slave instance.
     *
     * Instead we take the other branch of the IF statement setting an expire
     * (possibly in the past) and wait for an explicit DEL from the master. */
    if (when <= mstime() && !server.loading && !server.masterhost) {
        robj *aux;

        serverAssertWithInfo(c,key,dbDelete(c->db,key));
        server.dirty++;

        /* Replicate/AOF this as an explicit DEL. */
        aux = createStringObject("DEL",3);
        rewriteClientCommandVector(c,2,aux,key);
        decrRefCount(aux);
        signalModifiedKey(c->db,key);
        notifyKeyspaceEvent(NOTIFY_GENERIC,"del",key,c->db->id);
        addReply(c, shared.cone);
        return;
    } else {
        setExpire(c->db,key,when);
        addReply(c,shared.cone);
        signalModifiedKey(c->db,key);
        notifyKeyspaceEvent(NOTIFY_GENERIC,"expire",key,c->db->id);
        server.dirty++;
        return;
    }
}

void expireCommand(client *c) {
    expireGenericCommand(c,mstime(),UNIT_SECONDS);
}

// EXPIREAT key timestamp
// EXPIREAT 命令实现
void expireatCommand(client *c) {
    expireGenericCommand(c,0,UNIT_SECONDS);
}

// PEXPIRE key milliseconds
// PEXPIRE 命令实现
void pexpireCommand(client *c) {
    expireGenericCommand(c,mstime(),UNIT_MILLISECONDS);
}

void ttlGenericCommand(client *c, int output_ms) {
    long long expire, ttl = -1;

    /* If the key does not exist at all, return -2 */
    if (lookupKeyReadWithFlags(c->db,c->argv[1],LOOKUP_NOTOUCH) == NULL) {
        addReplyLongLong(c,-2);
        return;
    }
    /* The key exists. Return -1 if it has no expire, or the actual
     * TTL value otherwise. */
    expire = getExpire(c->db,c->argv[1]);
    if (expire != -1) {
        ttl = expire-mstime();
        if (ttl < 0) ttl = 0;
    }
    if (ttl == -1) {
        addReplyLongLong(c,-1);
    } else {
        addReplyLongLong(c,output_ms ? ttl : ((ttl+500)/1000));
    }
}

void ttlCommand(client *c) {
    ttlGenericCommand(c, 0);
}

/* Slot to Key API. This is used by Redis Cluster in order to obtain in
 * a fast way a key that belongs to a specified hash slot. This is useful
 * while rehashing the cluster. */
// 将key添加到槽里
void slotToKeyAdd(robj *key) {
	//// 计算所属的槽
	//unsigned int hashslot = keyHashSlot(key->ptr,sdslen(key->ptr));

	//// 将槽slot作为分值，key作为成员，添加到跳跃表中
	//zslInsert(server.cluster->slots_to_keys,hashslot,key);
	//incrRefCount(key);
}


// 从槽中删除指定的key
void slotToKeyDel(robj *key) {
    // 计算所属的槽
    unsigned int hashslot = keyHashSlot(key->ptr,sdslen(key->ptr));

    // 从跳跃表中删除
    zslDelete(server.cluster->slots_to_keys,hashslot,key);
}

void slotToKeyFlush(void) {
    zslFree(server.cluster->slots_to_keys);
    server.cluster->slots_to_keys = zslCreate();
}

