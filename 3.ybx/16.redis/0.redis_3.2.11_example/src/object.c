#include "server.h"
#include <math.h>
#include <ctype.h>

#ifdef __CYGWIN__
#define strtold(a,b) ((long double)strtod((a),(b)))
#endif

robj *createObject(int type, void *ptr) {
    robj *o = zmalloc(sizeof(*o));
    o->type = type;
    o->encoding = OBJ_ENCODING_RAW;
    o->ptr = ptr;
    o->refcount = 1;

    /* Set the LRU to the current lruclock (minutes resolution). */
    o->lru = LRU_CLOCK();
    return o;
}

/* Create a string object with encoding OBJ_ENCODING_RAW, that is a plain
 * string object where o->ptr points to a proper sds string. */
robj *createRawStringObject(const char *ptr, size_t len) {
    return createObject(OBJ_STRING,sdsnewlen(ptr,len));
}

/* Create a string object with encoding OBJ_ENCODING_EMBSTR, that is
 * an object where the sds string is actually an unmodifiable string
 * allocated in the same chunk as the object itself. */
robj *createEmbeddedStringObject(const char *ptr, size_t len) {
    robj *o = zmalloc(sizeof(robj)+sizeof(struct sdshdr8)+len+1);
    struct sdshdr8 *sh = (void*)(o+1);

    o->type = OBJ_STRING;
    o->encoding = OBJ_ENCODING_EMBSTR;
    o->ptr = sh+1;
    o->refcount = 1;
    o->lru = LRU_CLOCK();

    sh->len = len;
    sh->alloc = len;
    sh->flags = SDS_TYPE_8;
    if (ptr) {
        memcpy(sh->buf,ptr,len);
        sh->buf[len] = '\0';
    } else {
        memset(sh->buf,0,len+1);
    }
    return o;
}

/* Create a string object with EMBSTR encoding if it is smaller than
 * REIDS_ENCODING_EMBSTR_SIZE_LIMIT, otherwise the RAW encoding is
 * used.
 *
 * The current limit of 39 is chosen so that the biggest string object
 * we allocate as EMBSTR will still fit into the 64 byte arena of jemalloc. */
#define OBJ_ENCODING_EMBSTR_SIZE_LIMIT 44
robj *createStringObject(const char *ptr, size_t len) {
    if (len <= OBJ_ENCODING_EMBSTR_SIZE_LIMIT)
        return createEmbeddedStringObject(ptr,len);
    else
        return createRawStringObject(ptr,len);
}

//创建字符串对象，根据整数值
robj *createStringObjectFromLongLong(long long value) {
    robj *o;

    //redis中[0, 10000)内的整数是共享的
    if (value >= 0 && value < OBJ_SHARED_INTEGERS) {    //如果value属于redis共享整数的范围
        incrRefCount(shared.integers[value]);           //引用计数加1
        o = shared.integers[value];                     //返回一个编码类型为OBJ_ENCODING_INT的字符串对象

    //如果不在共享整数的范围
    } else {
        if (value >= LONG_MIN && value <= LONG_MAX) {   //value在long类型所表示的范围内
            o = createObject(OBJ_STRING, NULL);         //创建对象
            o->encoding = OBJ_ENCODING_INT;             //编码类型为OBJ_ENCODING_INT
            o->ptr = (void*)((long)value);              //指向这个value值
        } else {
            //value不在long类型所表示的范围内，将long long类型的整数转换为字符串
            //编码类型为OBJ_ENCODING_RAW
            o = createObject(OBJ_STRING,sdsfromlonglong(value));
        }
    }
    return o;
}

//创建一个quicklist编码的列表对象
robj *createQuicklistObject(void) {
    quicklist *l = quicklistCreate();       //创建一个quicklist
    robj *o = createObject(OBJ_LIST,l);     //创建一个对象，对象的数据类型为OBJ_LIST
    o->encoding = OBJ_ENCODING_QUICKLIST;   //对象的编码类型OBJ_ENCODING_QUICKLIST
    return o;
}

//创建一个ht编码的集合对象
robj *createSetObject(void) {
    dict *d = dictCreate(&setDictType,NULL);//创建一个字典
    robj *o = createObject(OBJ_SET,d);      //创建一个对象，对象的数据类型为OBJ_SET
    o->encoding = OBJ_ENCODING_HT;          //对象的编码类型OBJ_ENCODING_HT
    return o;
}

//创建一个intset编码的集合对象
robj *createIntsetObject(void) {
    intset *is = intsetNew();               //创建一个整数集合
    robj *o = createObject(OBJ_SET,is);     //创建一个对象，对象的数据类型为OBJ_SET
    o->encoding = OBJ_ENCODING_INTSET;      //对象的编码类型OBJ_ENCODING_INTSET
    return o;
}

//创建一个ziplist编码的哈希对象
robj *createHashObject(void) {
    unsigned char *zl = ziplistNew();       //创建一个ziplist
    robj *o = createObject(OBJ_HASH, zl);   //创建一个对象，对象的数据类型为OBJ_HASH
    o->encoding = OBJ_ENCODING_ZIPLIST;     //对象的编码类型OBJ_ENCODING_ZIPLIST
    return o;
}

void freeStringObject(robj *o) {
    if (o->encoding == OBJ_ENCODING_RAW) {
        sdsfree(o->ptr);
    }
}

void freeListObject(robj *o) {
    if (o->encoding == OBJ_ENCODING_QUICKLIST) {
        quicklistRelease(o->ptr);
    } else {
        serverPanic("Unknown list encoding type");
    }
}

//释放集合对象ptr指向的对象
void freeSetObject(robj *o) {
    switch (o->encoding) {
    case OBJ_ENCODING_HT:
        dictRelease((dict*) o->ptr);
        break;
    case OBJ_ENCODING_INTSET:
        zfree(o->ptr);
        break;
    default:
        serverPanic("Unknown set encoding type");
    }
}

void freeZsetObject(robj *o) {
    zset *zs;
    switch (o->encoding) {
    case OBJ_ENCODING_SKIPLIST:
        zs = o->ptr;
        dictRelease(zs->dict);
        zslFree(zs->zsl);
        zfree(zs);
        break;
    case OBJ_ENCODING_ZIPLIST:
        zfree(o->ptr);
        break;
    default:
        serverPanic("Unknown sorted set encoding");
    }
}

void incrRefCount(robj *o) {
    o->refcount++;
}

void decrRefCount(robj *o) {
    if (o->refcount <= 0) serverPanic("decrRefCount against refcount <= 0");
    if (o->refcount == 1) {
        switch(o->type) {
        case OBJ_STRING: freeStringObject(o); break;
        case OBJ_LIST: freeListObject(o); break;
        case OBJ_SET: freeSetObject(o); break;
        case OBJ_ZSET: freeZsetObject(o); break;
        //case OBJ_HASH: freeHashObject(o); break;
        default: serverPanic("Unknown object type"); break;//为了方便改的
        }
        zfree(o);
    } else {
        o->refcount--;
    }
}

void decrRefCountVoid(void *o) {
    decrRefCount(o);
}

/* Equal string objects return 1 if the two objects are the same from the
 * point of view of a string comparison, otherwise 0 is returned. Note that
 * this function is faster then checking for (compareStringObject(a,b) == 0)
 * because it can perform some more optimization. */
int equalStringObjects(robj *a, robj *b) {
    if (a->encoding == OBJ_ENCODING_INT &&
        b->encoding == OBJ_ENCODING_INT){
        /* If both strings are integer encoded just check if the stored
         * long is the same. */
        return a->ptr == b->ptr;
    } else {
        return compareStringObjects(a,b) == 0;
    }
}

//判断对象的ptr指向的值能否转换为long long类型，如果可以保存在llval中
int isObjectRepresentableAsLongLong(robj *o, long long *llval) {
    //serverAssertWithInfo(NULL,o,o->type == OBJ_STRING);
    if (o->encoding == OBJ_ENCODING_INT) {      //如果本身就是整数
        if (llval) *llval = (long) o->ptr;
        return C_OK;                            //成功返回0
    } else {
        //字符串转换为longlong类型，成功返回0，失败返回-1
        return string2ll(o->ptr,sdslen(o->ptr),llval) ? C_OK : C_ERR;
    }
}



/* Duplicate a string object, with the guarantee that the returned object
 * has the same encoding as the original one.
 *
 * This function also guarantees that duplicating a small integere object
 * (or a string object that contains a representation of a small integer)
 * will always result in a fresh object that is unshared (refcount == 1).
 *
 * The resulting object always has refcount set to 1. */
//返回 复制的o对象的副本的地址，且创建的对象非共享
robj *dupStringObject(robj *o) {
    robj *d;

    serverAssert(o->type == OBJ_STRING);    //一定是OBJ_STRING类型

    switch(o->encoding) {                   //根据不同的编码类型
    case OBJ_ENCODING_RAW:
        return createRawStringObject(o->ptr,sdslen(o->ptr));        //创建的对象非共享
    case OBJ_ENCODING_EMBSTR:
        return createEmbeddedStringObject(o->ptr,sdslen(o->ptr));   //创建的对象非共享
    case OBJ_ENCODING_INT:                  //整数编码类型
        d = createObject(OBJ_STRING, NULL); //即使是共享整数范围内的整数，创建的对象也是非共享的
        d->encoding = OBJ_ENCODING_INT;
        d->ptr = o->ptr;
        return d;
    default:
        serverPanic("Wrong encoding.");
        break;
    }
}




/* Try to encode a string object in order to save space */
//
robj *tryObjectEncoding(robj *o) {
    long value;
    sds s = o->ptr;
    size_t len;

    /* Make sure this is a string object, the only type we encode
     * in this function. Other types use encoded memory efficient
     * representations but are handled by the commands implementing
     * the type. */
    serverAssertWithInfo(NULL,o,o->type == OBJ_STRING);

    //如果字符串对象的编码类型为RAW或EMBSTR时，才对其重新编码
    if (!sdsEncodedObject(o)) return o;

    //如果refcount大于1，则说明对象的ptr指向的值是共享的，不对共享对象进行编码
     if (o->refcount > 1) return o;

    /* Check if we can represent this string as a long integer.
     * Note that we are sure that a string larger than 20 chars is not
     * representable as a 32 nor 64 bit integer. */
    len = sdslen(s);            //获得字符串s的长度

    //如果len小于等于20，表示符合long long可以表示的范围，
	//且可以转换为long 类型的字符串进行编码
    if (len <= 20 && string2l(s,len,&value)) {
        /* This object is encodable as a long. Try to use a shared object.
         * Note that we avoid using shared integers when maxmemory is used
         * because every object needs to have a private LRU field for the LRU
         * algorithm to work well. */
        if ((server.maxmemory == 0 ||
             (server.maxmemory_policy != MAXMEMORY_VOLATILE_LRU &&
              server.maxmemory_policy != MAXMEMORY_ALLKEYS_LRU)) &&
            value >= 0 &&
            value < OBJ_SHARED_INTEGERS)    //如果value处于共享整数的范围内
        {
            decrRefCount(o);                //原对象的引用计数减1，释放对象
            incrRefCount(shared.integers[value]); //增加共享对象的引用计数
            return shared.integers[value];
			//返回一个编码为整数的字符串对象
        } else {        //如果不处于共享整数的范围
            if (o->encoding == OBJ_ENCODING_RAW) sdsfree(o->ptr);
			//释放编码为OBJ_ENCODING_RAW的对象
            o->encoding = OBJ_ENCODING_INT;     //转换为OBJ_ENCODING_INT编码
            o->ptr = (void*) value;             //指针ptr指向value对象
            return o;
        }
    }

    /* If the string is small and is still RAW encoded,
     * try the EMBSTR encoding which is more efficient.
     * In this representation the object and the SDS string are allocated
     * in the same chunk of memory to save space and cache misses. */
    //如果len小于44，44是最大的编码为EMBSTR类型的字符串对象长度
    if (len <= OBJ_ENCODING_EMBSTR_SIZE_LIMIT) {
        robj *emb;

        if (o->encoding == OBJ_ENCODING_EMBSTR) return o;   //将RAW对象转换为 OBJ_ENCODING_EMBSTR编码类型
        emb = createEmbeddedStringObject(s,sdslen(s)); //创建一个编码类型为 OBJ_ENCODING_EMBSTR的字符串对象
        decrRefCount(o);    //释放之前的对象
        return emb;
    }

    /* We can't encode the object...
     *
     * Do the last try, and at least optimize the SDS string inside
     * the string object to require little space, in case there
     * is more than 10% of free space at the end of the SDS string.
     *
     * We do that only for relatively large strings as this branch
     * is only entered if the length of the string is greater than
     * OBJ_ENCODING_EMBSTR_SIZE_LIMIT. */
    //无法进行编码，但是如果s的未使用的空间大于使用空间的10分之1
    if (o->encoding == OBJ_ENCODING_RAW &&
        sdsavail(s) > len/10)
    {
        o->ptr = sdsRemoveFreeSpace(o->ptr);    //释放所有的未使用空间
    }

    /* Return the original object. */
    return o;
}


/* Get a decoded version of an encoded object (returned as a new object).
 * If the object is already raw-encoded just increment the ref count. */
//将对象是整型的解码为字符串并返回，如果是字符串编码则之间返回输入对象，只需增加引用计数
robj *getDecodedObject(robj *o) {
    robj *dec;

    if (sdsEncodedObject(o)) {  //如果是OBJ_ENCODING_RAW或OBJ_ENCODING_EMBSTR类型的对象
        incrRefCount(o);        //增加应用计数
        return o;
    }
    if (o->type == OBJ_STRING && o->encoding == OBJ_ENCODING_INT) { //如果是整数对象
        char buf[32];

        ll2string(buf,32,(long)o->ptr); //将整数转换为字符串
        dec = createStringObject(buf,strlen(buf));  //创建一个字符串对象
        return dec;
    } else {
        serverPanic("Unknown encoding type");
    }
}

#define REDIS_COMPARE_BINARY (1<<0)
#define REDIS_COMPARE_COLL (1<<1)

int compareStringObjectsWithFlags(robj *a, robj *b, int flags) {
    //serverAssertWithInfo(NULL,a,a->type == OBJ_STRING && b->type == OBJ_STRING);
    char bufa[128], bufb[128], *astr, *bstr;
    size_t alen, blen, minlen;

    if (a == b) return 0;
    if (sdsEncodedObject(a)) {
        astr = a->ptr;
        alen = sdslen(astr);
    } else {
        alen = ll2string(bufa,sizeof(bufa),(long) a->ptr);
        astr = bufa;
    }
    if (sdsEncodedObject(b)) {
        bstr = b->ptr;
        blen = sdslen(bstr);
    } else {
        blen = ll2string(bufb,sizeof(bufb),(long) b->ptr);
        bstr = bufb;
    }
    if (flags & REDIS_COMPARE_COLL) {
        return strcoll(astr,bstr);
    } else {
        int cmp;

        minlen = (alen < blen) ? alen : blen;
        cmp = memcmp(astr,bstr,minlen);
        if (cmp == 0) return alen-blen;
        return cmp;
    }
}

/* Wrapper for compareStringObjectsWithFlags() using binary comparison. */
int compareStringObjects(robj *a, robj *b) {
    return compareStringObjectsWithFlags(a,b,REDIS_COMPARE_BINARY);
}


size_t stringObjectLen(robj *o) {
    serverAssertWithInfo(NULL,o,o->type == OBJ_STRING);
    if (sdsEncodedObject(o)) {
        return sdslen(o->ptr);
    } else {
        return sdigits10((long)o->ptr);
    }
}

//从对象中将字符串值转换为long long并存储在target中
int getLongLongFromObject(robj *o, long long *target) {
	long long value;
	if (o == NULL) {    //对象不存在
		value = 0;
	} else {
		serverAssertWithInfo(NULL,o,o->type == OBJ_STRING);
		//assert(o->type == OBJ_STRING);
		//如果是字符串编码的两种类型
		if (sdsEncodedObject(o)) {
			//转换失败发送-1，成功保存值到value中
			if (string2ll(o->ptr,sdslen(o->ptr),&value) == 0) return C_ERR;
		} else if (o->encoding == OBJ_ENCODING_INT) {   //整型编码
			value = (long)o->ptr;                       //保存整数值
		} else {
			serverPanic("Unknown string encoding");
		}
	}
	if (target) *target = value;        //将值存到传入参数中，返回0成功
	return C_OK;
}

/*
 * 尝试从对象 o 中取出整数值，
 * 或者尝试将对象 o 中的值转换为整数值，
 * 并将这个得出的整数值保存到 *target 。
 *
 * 如果取出/转换成功的话，返回 REDIS_OK 。
 * 否则，返回 REDIS_ERR ，并向客户端发送一条出错回复。
 *
 * T = O(N)
 */
int getLongLongFromObjectOrReply(client *c, robj *o, long long *target, const char *msg) {
    long long value;
    if (getLongLongFromObject(o, &value) != C_OK) {
        if (msg != NULL) {
            addReplyError(c,(char*)msg);
        } else {
            addReplyError(c,"value is not an integer or out of range");
        }
        return C_ERR;
    }
    *target = value;
    return C_OK;
}

int getLongFromObjectOrReply(client *c, robj *o, long *target, const char *msg) {
    long long value;

    if (getLongLongFromObjectOrReply(c, o, &value, msg) != C_OK) return C_ERR;
    if (value < LONG_MIN || value > LONG_MAX) {
        if (msg != NULL) {
            addReplyError(c,(char*)msg);
        } else {
            addReplyError(c,"value is out of range");
        }
        return C_ERR;
    }
    *target = value;
    return C_OK;
}
