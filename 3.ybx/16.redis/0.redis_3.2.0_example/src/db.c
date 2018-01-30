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

// 以写操作取出key的值对象，不更新是否命中的信息
robj *lookupKeyWrite(redisDb *db, robj *key) {
    expireIfNeeded(db,key);
    return lookupKey(db,key,LOOKUP_NONE);
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

int selectDb(client *c, int id) {
    if (id < 0 || id >= server.dbnum)
        return C_ERR;
    c->db = &server.db[id];
    return C_OK;
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

// 当key被修改，调用该函数
void signalModifiedKey(redisDb *db, robj *key) {
UNUSED(db);
UNUSED(key);
    //touchWatchedKey(db,key);
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

