#include "server.h"
#include <math.h>


/* Encode given objects in-place when the hash uses a dict. */
// 对键和值的对象尝试进行优化编码以节约内存
void hashTypeTryObjectEncoding(robj *subject, robj **o1, robj **o2) {
    // 如果当前subject对象的编码为OBJ_ENCODING_HT，则对o1对象和o2对象进行尝试优化编码
    if (subject->encoding == OBJ_ENCODING_HT) {
        if (o1) *o1 = tryObjectEncoding(*o1);
        if (o2) *o2 = tryObjectEncoding(*o2);
    }
}


/* Get the value from a ziplist encoded hash, identified by field.
 * Returns -1 when the field cannot be found. */
//从一个编码为ziplist的哈希对象中，取出对应field的值，并将值保存到vstr或vll中，没有找到field返回-1
int hashTypeGetFromZiplist(robj *o, robj *field,
                           unsigned char **vstr,
                           unsigned int *vlen,
                           long long *vll)
{
    unsigned char *zl, *fptr = NULL, *vptr = NULL;
    int ret;

    // 确保编码为OBJ_ENCODING_ZIPLIST
    serverAssert(o->encoding == OBJ_ENCODING_ZIPLIST);

    // 将field编码转换为字符串类型的两种编码EMBSTR或RAW的对象
    field = getDecodedObject(field);

    zl = o->ptr;
    // 返回头节点entry的地址，从头开始便利
    fptr = ziplistIndex(zl, ZIPLIST_HEAD);
    if (fptr != NULL) {
        // 在ziplist中查找和field值相等的entry节点，该节点对应key
        fptr = ziplistFind(fptr, field->ptr, sdslen(field->ptr), 1);
        if (fptr != NULL) {
            /* Grab pointer to the value (fptr points to the field) */
            // 找到下一个entry节点的地址，该节点对应value
            vptr = ziplistNext(zl, fptr);
            serverAssert(vptr != NULL);
        }
    }

    decrRefCount(field);    //释放field对象

    if (vptr != NULL) {
        // 将对应value节点的值保存在传入的参数中
        ret = ziplistGet(vptr, vstr, vlen, vll);
        serverAssert(ret);
        return 0;   //找到返回0
    }

    return -1;
}


/* Get the value from a hash table encoded hash, identified by field.
 * Returns -1 when the field cannot be found. */
// 从一个编码为OBJ_ENCODING_HT的哈希对象中，取出对应field的值，并将值保存到value对象中，没有找到field返回-1
int hashTypeGetFromHashTable(robj *o, robj *field, robj **value) {
    dictEntry *de;

    serverAssert(o->encoding == OBJ_ENCODING_HT);

    // 在字典中查找该field键
    de = dictFind(o->ptr, field);
    // field键不存在返回-1
    if (de == NULL) return -1;
    // 从该键中提取对应的值保存在*value中
    *value = dictGetVal(de);
    return 0;
}

// 将field-value添加到哈希对象中，返回1
// 如果field存在更新新的值，返回0
int hashTypeSet(robj *o, robj *field, robj *value) {
	int update = 0;

	// 如果是ziplist类型
	if (o->encoding == OBJ_ENCODING_ZIPLIST) {
		unsigned char *zl, *fptr, *vptr;

		// 如果field-value是整数，则解码为字符串类型
		field = getDecodedObject(field);
		value = getDecodedObject(value);

		zl = o->ptr;
		// 遍历整个ziplist，得到头entry节点的地址
		fptr = ziplistIndex(zl, ZIPLIST_HEAD);
		if (fptr != NULL) {
			// 在ziplist中查找并返回和field相等的entry节点
			fptr = ziplistFind(fptr, field->ptr, sdslen(field->ptr), 1);

			// 如果field存在
			if (fptr != NULL) {
				/* Grab pointer to the value (fptr points to the field) */
				// 返回当value的entry地址，也就是field的下一个entry
				vptr = ziplistNext(zl, fptr);
				serverAssert(vptr != NULL);
				update = 1; // 设置更新的标志

				/* Delete value */
				// 将找到的value删除
				zl = ziplistDelete(zl, &vptr);

				/* Insert new value */
				// 插入新的value节点
				zl = ziplistInsert(zl, vptr, value->ptr, sdslen(value->ptr));
			}
		}

		// 如果没有找到field
		if (!update) {
			/* Push new field/value pair onto the tail of the ziplist */
			// 讲field和value按序压入到ziplist中
			zl = ziplistPush(zl, field->ptr, sdslen(field->ptr), ZIPLIST_TAIL);
			zl = ziplistPush(zl, value->ptr, sdslen(value->ptr), ZIPLIST_TAIL);
		}
		// 更新哈希对象
		o->ptr = zl;
		// 释放临时的field-value
		decrRefCount(field);
		decrRefCount(value);

		/* Check if the ziplist needs to be converted to a hash table */
		// 在配置的条件下，如果能进行优化编码以便节约内存
		if (hashTypeLength(o) > server.hash_max_ziplist_entries)
			hashTypeConvert(o, OBJ_ENCODING_HT);

		// 如果是添加到字典
	} else if (o->encoding == OBJ_ENCODING_HT) {
		// 插入或替换字典的field-value对，插入返回1，替换返回0
		if (dictReplace(o->ptr, field, value)) { /* Insert */
			incrRefCount(field);    //如果是插入成功，则释放field
		} else { /* Update */
			update = 1;     //设置更新的标志
		}
		incrRefCount(value);    //释放value对象
	} else {
		serverPanic("Unknown hash encoding");
	}
	return update;  //更新返回1，替换返回0
}

/* Return the number of elements in a hash. */
// 返回哈希对象中的键值对个数
unsigned long hashTypeLength(robj *o) {
    unsigned long length = ULONG_MAX;

    // 返回ziplist的entry节点个数的一半，则为一对field-value的个数
    if (o->encoding == OBJ_ENCODING_ZIPLIST) {
        length = ziplistLen(o->ptr) / 2;

    // 返回字典的大小
    } else if (o->encoding == OBJ_ENCODING_HT) {
        length = dictSize((dict*)o->ptr);
    } else {
        serverPanic("Unknown hash encoding");
    }

    return length;
}


// 返回一个初始化的哈希类型的迭代器
hashTypeIterator *hashTypeInitIterator(robj *subject) {
	// 分配空间初始化成员
	hashTypeIterator *hi = zmalloc(sizeof(hashTypeIterator));
	hi->subject = subject;
	hi->encoding = subject->encoding;

	// 根据不同的编码设置不同的成员
	if (hi->encoding == OBJ_ENCODING_ZIPLIST) {
		hi->fptr = NULL;
		hi->vptr = NULL;
	} else if (hi->encoding == OBJ_ENCODING_HT) {
		// 初始化一个字典迭代器返回给di成员
		hi->di = dictGetIterator(subject->ptr);
	} else {
		serverPanic("Unknown hash encoding");
	}

	return hi;
}

// 释放哈希类型迭代器空间
void hashTypeReleaseIterator(hashTypeIterator *hi) {
	// 如果是字典，则需要先释放字典迭代器的空间
	if (hi->encoding == OBJ_ENCODING_HT) {
		dictReleaseIterator(hi->di);
	}

	zfree(hi);
}



/* Move to the next entry in the hash. Return C_OK when the next entry
 * could be found and C_ERR when the iterator reaches the end. */
//讲哈希类型迭代器指向哈希对象中的下一个节点
int hashTypeNext(hashTypeIterator *hi) {
	// 迭代ziplist
	if (hi->encoding == OBJ_ENCODING_ZIPLIST) {
		unsigned char *zl;
		unsigned char *fptr, *vptr;

		// 备份迭代器的成员信息
		zl = hi->subject->ptr;
		fptr = hi->fptr;
		vptr = hi->vptr;

		// field的指针为空，则指向第一个entry，只在第一次执行时，初始化指针
		if (fptr == NULL) {
			/* Initialize cursor */
			serverAssert(vptr == NULL);
			fptr = ziplistIndex(zl, 0);
		} else {
			/* Advance cursor */
			// 获取value节点的下一个entry地址，即为下一个field的地址
			serverAssert(vptr != NULL);
			fptr = ziplistNext(zl, vptr);
		}
		// 迭代完毕或返回C_ERR
		if (fptr == NULL) return C_ERR;

		/* Grab pointer to the value (fptr points to the field) */
		// 保存下一个value的地址
		vptr = ziplistNext(zl, fptr);
		serverAssert(vptr != NULL);

		/* fptr, vptr now point to the first or next pair */
		// 更新迭代器的成员信息
		hi->fptr = fptr;
		hi->vptr = vptr;

		// 如果是迭代字典
	} else if (hi->encoding == OBJ_ENCODING_HT) {
		// 得到下一个字典节点的地址
		if ((hi->de = dictNext(hi->di)) == NULL) return C_ERR;
	} else {
		serverPanic("Unknown hash encoding");
	}
	return C_OK;
}

// 从ziplist类型的哈希类型迭代器中获取对应的field或value，保存在参数中
void hashTypeCurrentFromZiplist(hashTypeIterator *hi, int what,
                                unsigned char **vstr,
                                unsigned int *vlen,
                                long long *vll)
{
    int ret;

    serverAssert(hi->encoding == OBJ_ENCODING_ZIPLIST);

    // 如果获取field
    if (what & OBJ_HASH_KEY) {
        // 保存键到参数中
        ret = ziplistGet(hi->fptr, vstr, vlen, vll);
        serverAssert(ret);
    } else {
        // 保存值到参数中
        ret = ziplistGet(hi->vptr, vstr, vlen, vll);
        serverAssert(ret);
    }
}

//从ht字典类型的哈希类型迭代器中获取对应的field或value，保存在参数中
void hashTypeCurrentFromHashTable(hashTypeIterator *hi, int what, robj **dst) {
    serverAssert(hi->encoding == OBJ_ENCODING_HT);

    // 如果获取field
    if (what & OBJ_HASH_KEY) {
        // 保存键到参数中
        *dst = dictGetKey(hi->de);
    } else {
        // 保存值到参数中
        *dst = dictGetVal(hi->de);
    }
}




/* A non copy-on-write friendly but higher level version of hashTypeCurrent*()
 * that returns an object with incremented refcount (or a new object). It is up
 * to the caller to decrRefCount() the object if no reference is retained. */
// 从哈希类型的迭代器中获取键或值
robj *hashTypeCurrentObject(hashTypeIterator *hi, int what) {
    robj *dst;

    // 如果从ziplist中获取
    if (hi->encoding == OBJ_ENCODING_ZIPLIST) {
        unsigned char *vstr = NULL;
        unsigned int vlen = UINT_MAX;
        long long vll = LLONG_MAX;

        // 获取键或值
        hashTypeCurrentFromZiplist(hi, what, &vstr, &vlen, &vll);

        // 为获取的键或值创建字符串对象，如果是整数需要转换为字符串
        if (vstr) {
            dst = createStringObject((char*)vstr, vlen);
        } else {
            dst = createStringObjectFromLongLong(vll);
        }
    // 从字典中获取
    } else if (hi->encoding == OBJ_ENCODING_HT) {
        // 获取键或值
        hashTypeCurrentFromHashTable(hi, what, &dst);
        // 增加键或值对象的引用计数
        incrRefCount(dst);
    } else {
        serverPanic("Unknown hash encoding");
    }
    return dst;
}



// 将一个ziplist类型的哈希对象，转换为enc类型的对象
void hashTypeConvertZiplist(robj *o, int enc) {
    serverAssert(o->encoding == OBJ_ENCODING_ZIPLIST);  //确保为ziplist类型的对象

    // 如果enc为OBJ_ENCODING_ZIPLIST则什么都不做
    if (enc == OBJ_ENCODING_ZIPLIST) {
        /* Nothing to do... */

    // 如果要转换为OBJ_ENCODING_HT类型
    } else if (enc == OBJ_ENCODING_HT) {
        hashTypeIterator *hi;
        dict *dict;
        int ret;

        // 创建并初始化一个哈希类型的迭代器
        hi = hashTypeInitIterator(o);
        // 创建一个新的字典并初始化
        dict = dictCreate(&hashDictType, NULL);

        // 遍历ziplist
        while (hashTypeNext(hi) != C_ERR) {
            robj *field, *value;

            // 取出哈希对象的键field，并且优化编码
            field = hashTypeCurrentObject(hi, OBJ_HASH_KEY);
            field = tryObjectEncoding(field);

            // 取出哈希对象的值value，并且优化编码
            value = hashTypeCurrentObject(hi, OBJ_HASH_VALUE);
            value = tryObjectEncoding(value);

            //讲field-value对添加到新创建的字典中
            ret = dictAdd(dict, field, value);

            if (ret != DICT_OK) {
                // 添加失败返回错误日志信息，并且中断程序
                serverLogHexDump(LL_WARNING,"ziplist with dup elements dump",
                    o->ptr,ziplistBlobLen(o->ptr));
                serverAssert(ret == DICT_OK);
            }
        }

        // 释放迭代器空间
        hashTypeReleaseIterator(hi);
        zfree(o->ptr);

        // 更新编码类型和值对象
        o->encoding = OBJ_ENCODING_HT;
        o->ptr = dict;

    } else {
        serverPanic("Unknown hash encoding");
    }
}

// 转换一个哈希对象的编码类型，enc指定新的编码类型
void hashTypeConvert(robj *o, int enc) {

    // 当前的ziplist转换为字典或不变
    if (o->encoding == OBJ_ENCODING_ZIPLIST) {
        hashTypeConvertZiplist(o, enc);

    // 当前是字典类型，不支持转换
    } else if (o->encoding == OBJ_ENCODING_HT) {
        serverPanic("Not implemented");
    } else {
        serverPanic("Unknown hash encoding");
    }
}

