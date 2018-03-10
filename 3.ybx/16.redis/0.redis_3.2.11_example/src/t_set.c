#include "server.h"

// 创建一个保存value的集合
robj *setTypeCreate(robj *value) {
	// 如果value对象的值可以转换为long long 类型的整数，则创建一个整数集合intset
	if (isObjectRepresentableAsLongLong(value,NULL) == C_OK)
		return createIntsetObject();

	// 否则创建一个哈希表类型的集合
	return createSetObject();
}

// 向subject集合中添加value，添加成功返回1，如果已经存在返回0
int setTypeAdd(robj *subject, robj *value) {
	long long llval;
	// 如果是字典构成的集合
	if (subject->encoding == OBJ_ENCODING_HT) {
		// 将value加入集合作为哈希键，哈希值为NULL
		if (dictAdd(subject->ptr,value,NULL) == DICT_OK) {
			incrRefCount(value);
			return 1;
		}
		// 如果是整数集合
	} else if (subject->encoding == OBJ_ENCODING_INTSET) {
		// 如果要添加的value可以转换为整数，则保存在llval中
		if (isObjectRepresentableAsLongLong(value,&llval) == C_OK) {
			uint8_t success = 0;
			// 将转换为整数的value加入集合，添加成功，讲success赋值为1
			subject->ptr = intsetAdd(subject->ptr,llval,&success);

			//如果添加成功
			if (success) {
				/* Convert to regular set when the intset contains
				 * too many entries. */
				// 查看整数集合的元素个数是否大于配置的最大个数
				if (intsetLen(subject->ptr) > server.set_max_intset_entries)
					// 如果超过则需要转换为字典类型的集合
					setTypeConvert(subject,OBJ_ENCODING_HT);
				return 1;
			}
		} else {
			/* Failed to get integer from object, convert to regular set. */
			// 不能将value转换为longlong可以表示整数，那么需要将intset类型转换成字典类型集合
			setTypeConvert(subject,OBJ_ENCODING_HT);

			/* The set *was* an intset and this value is not integer
			 * encodable, so dictAdd should always work. */
			// 确保执行向字典中添加了value
			serverAssertWithInfo(NULL,value,
					dictAdd(subject->ptr,value,NULL) == DICT_OK);
			incrRefCount(value);
			return 1;
		}
	} else {
		serverPanic("Unknown set encoding");
	}
	return 0;
}

// 从集合对象中删除一个值为value的元素，删除成功返回1，失败返回0
int setTypeRemove(robj *setobj, robj *value) {
    long long llval;
    // 从字典中删除一个元素
    if (setobj->encoding == OBJ_ENCODING_HT) {
        // 如果成功从字典中删除了元素，需要进行判断是否缩小字典的大小
        if (dictDelete(setobj->ptr,value) == DICT_OK) {
            if (htNeedsResize(setobj->ptr)) dictResize(setobj->ptr);
            return 1;
        }
    // 从整数集合中删除一个元素
    } else if (setobj->encoding == OBJ_ENCODING_INTSET) {
        // 如果value可以转换为整型值，则进行删除
        if (isObjectRepresentableAsLongLong(value,&llval) == C_OK) {
            int success;
            setobj->ptr = intsetRemove(setobj->ptr,llval,&success);
            if (success) return 1;
        }
    } else {
        serverPanic("Unknown set encoding");
    }
    return 0;   //删除失败返回0
}

// 集合中是否存在值为value的元素，存在返回1，否则返回0
int setTypeIsMember(robj *subject, robj *value) {
    long long llval;
    // 从字典中查找
    if (subject->encoding == OBJ_ENCODING_HT) {
        return dictFind((dict*)subject->ptr,value) != NULL;
    // 从整数集合中查找
    } else if (subject->encoding == OBJ_ENCODING_INTSET) {
        // 必须转换为整数类型进行查找
        if (isObjectRepresentableAsLongLong(value,&llval) == C_OK) {
            return intsetFind((intset*)subject->ptr,llval);
        }
    } else {
        serverPanic("Unknown set encoding");
    }
    return 0;
}

// 返回迭代器当前指向的元素对象的地址，需要手动释放返回的对象
robj *setTypeNextObject(setTypeIterator *si) {
    int64_t intele;
    robj *objele;
    int encoding;

    // 得到当前集合对象的编码类型
    encoding = setTypeNext(si,&objele,&intele);
    switch(encoding) {
        case -1:    return NULL;    //迭代完成
        case OBJ_ENCODING_INTSET:   //整数集合返回一个字符串类型的对象
            return createStringObjectFromLongLong(intele);
        case OBJ_ENCODING_HT:       //字典集合，返回共享的该对象
            incrRefCount(objele);
            return objele;
        default:
            serverPanic("Unsupported encoding");
    }
    return NULL; /* just to suppress warnings */
}

// 返回集合的元素数量
unsigned long setTypeSize(robj *subject) {
    // 返回字典中的节点数量
    if (subject->encoding == OBJ_ENCODING_HT) {
        return dictSize((dict*)subject->ptr);
    // 返回整数集合中的元素数量
    } else if (subject->encoding == OBJ_ENCODING_INTSET) {
        return intsetLen((intset*)subject->ptr);
    } else {
        serverPanic("Unknown set encoding");
    }
}

// 将集合对象的INTSET编码类型转换为enc类型
void setTypeConvert(robj *setobj, int enc) {
	setTypeIterator *si;
	//serverAssertWithInfo(NULL,setobj,setobj->type == OBJ_SET && \
		setobj->encoding == OBJ_ENCODING_INTSET);

	// 转换成OBJ_ENCODING_HT字典类型的编码
	if (enc == OBJ_ENCODING_HT) {
		int64_t intele;
		// 创建一个字典
		dict *d = dictCreate(&setDictType,NULL);
		robj *element;

		/* Presize the dict to avoid rehashing */
		// 扩展字典的大小
		dictExpand(d,intsetLen(setobj->ptr));

		/* To add the elements we extract integers and create redis objects */
		// 创建并初始化一个集合类型的迭代器
		si = setTypeInitIterator(setobj);
		// 迭代器整数集合
		while (setTypeNext(si,&element,&intele) != -1) {
			element = createStringObjectFromLongLong(intele);   //将当前集合中的元素转换为字符串类型对象
			serverAssertWithInfo(NULL,element,
					dictAdd(d,element,NULL) == DICT_OK);
		}
		// 释放迭代器空间
		setTypeReleaseIterator(si);

		// 设置转换后的集合对象的编码类型
		setobj->encoding = OBJ_ENCODING_HT;
		// 更新集合对象的值对象
		zfree(setobj->ptr);
		setobj->ptr = d;
	} else {
		serverPanic("Unsupported set conversion");
	}
}

setTypeIterator *setTypeInitIterator(robj *subject) {
    setTypeIterator *si = zmalloc(sizeof(setTypeIterator));
    si->subject = subject;
    si->encoding = subject->encoding;
    if (si->encoding == OBJ_ENCODING_HT) {
        si->di = dictGetIterator(subject->ptr);
    } else if (si->encoding == OBJ_ENCODING_INTSET) {
        si->ii = 0;
    } else {
        serverPanic("Unknown set encoding");
    }
    return si;
}

void setTypeReleaseIterator(setTypeIterator *si) {
    if (si->encoding == OBJ_ENCODING_HT)
        dictReleaseIterator(si->di);
    zfree(si);
}

int setTypeNext(setTypeIterator *si, robj **objele, int64_t *llele) {
    if (si->encoding == OBJ_ENCODING_HT) {
        dictEntry *de = dictNext(si->di);
        if (de == NULL) return -1;
        *objele = dictGetKey(de);
        *llele = -123456789; /* Not needed. Defensive. */
    } else if (si->encoding == OBJ_ENCODING_INTSET) {
        if (!intsetGet(si->subject->ptr,si->ii++,llele))
            return -1;
        *objele = NULL; /* Not needed. Defensive. */
    } else {
        serverPanic("Wrong set encoding in setTypeNext");
    }
    return si->encoding;
}

// 返回s2集合减去s1集合的元素数量只差
int qsortCompareSetsByRevCardinality(const void *s1, const void *s2) {
    robj *o1 = *(robj**)s1, *o2 = *(robj**)s2;

    return  (o2 ? setTypeSize(o2) : 0) - (o1 ? setTypeSize(o1) : 0);
}
