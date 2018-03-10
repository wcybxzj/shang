#include "server.h"
#include <math.h>

//创建一个skiplist编码的有序集合对象
robj *createZsetObject(void) {
    zset *zs = zmalloc(sizeof(*zs));
    robj *o;

    zs->dict = dictCreate(&zsetDictType,NULL);  //创建一个字典
    zs->zsl = zslCreate();                      //创建一个跳跃表
    o = createObject(OBJ_ZSET,zs);              //创建一个对象，对象的数据类型为OBJ_ZSET
    o->encoding = OBJ_ENCODING_SKIPLIST;        //对象的编码类型OBJ_ENCODING_SKIPLIST
    return o;
}

//创建一个ziplist编码的有序集合对象
robj *createZsetZiplistObject(void) {
    unsigned char *zl = ziplistNew();           //创建一个ziplist
    robj *o = createObject(OBJ_ZSET,zl);        //创建一个对象，对象的数据类型为OBJ_ZSET
    o->encoding = OBJ_ENCODING_ZIPLIST;         //对象的编码类型OBJ_ENCODING_ZIPLIST
    return o;
}

zskiplistNode *zslCreateNode(int level, double score, robj *obj) {
    zskiplistNode *zn = zmalloc(sizeof(*zn)+level*sizeof(struct zskiplistLevel));
    zn->score = score;
    zn->obj = obj;
    return zn;
}

zskiplist *zslCreate(void) {
    int j;
    zskiplist *zsl;

    zsl = zmalloc(sizeof(*zsl));
    zsl->level = 1;
    zsl->length = 0;
    zsl->header = zslCreateNode(ZSKIPLIST_MAXLEVEL,0,NULL);
    for (j = 0; j < ZSKIPLIST_MAXLEVEL; j++) {
        zsl->header->level[j].forward = NULL;
        zsl->header->level[j].span = 0;
    }
    zsl->header->backward = NULL;
    zsl->tail = NULL;
    return zsl;
}

void zslFreeNode(zskiplistNode *node) {
    decrRefCount(node->obj);
    zfree(node);
}

void zslFree(zskiplist *zsl) {//释放跳跃表表头zsl，以及跳跃表节点
    zskiplistNode *node = zsl->header->level[0].forward, *next;

    zfree(zsl->header);//释放跳跃表的头节点
    while(node) {//释放其他节点
        next = node->level[0].forward;//备份下一个节点地址
        zslFreeNode(node);//释放节点空间
        node = next;//指向下一个节点
    }
    zfree(zsl);//释放表头
}

int zslRandomLevel(void) {
    int level = 1;
    while ((random()&0xFFFF) < (ZSKIPLIST_P * 0xFFFF))
        level += 1;
    return (level<ZSKIPLIST_MAXLEVEL) ? level : ZSKIPLIST_MAXLEVEL;
}

int ybx_zslRandomLevel(void) {
    int level = 1;
    while ((random()&0xFFFF) < (0.5* 0xFFFF))
        level += 1;
    return (level<ZSKIPLIST_MAXLEVEL) ? level : ZSKIPLIST_MAXLEVEL;
}

zskiplistNode *zslInsert(zskiplist *zsl, double score, robj *obj) {
	// updata[]数组记录每一层位于插入节点的前一个节点
	zskiplistNode *update[ZSKIPLIST_MAXLEVEL], *x;
	// rank[]记录每一层位于插入节点的前一个节点的排名
	unsigned int rank[ZSKIPLIST_MAXLEVEL];
	int i, level;
	serverAssert(!isnan(score));
	x = zsl->header; // 表头节点
	// 从最高层开始查找
	for (i = zsl->level-1; i >= 0; i--) {
		// 存储rank值是为了交叉快速地到达插入位置
		rank[i] = i == (zsl->level-1) ? 0 : rank[i+1];
		// 前向指针不为空，前置指针的分值小于score或当前向指针的分值等// 于空但成员对象不等于o的情况下，继续向前查找
		while (x->level[i].forward &&
				(x->level[i].forward->score < score ||
				 (x->level[i].forward->score == score &&
				  compareStringObjects(x->level[i].forward->obj,obj) < 0))) {
			rank[i] += x->level[i].span;
			x = x->level[i].forward;
		}
		// 存储当前层上位于插入节点的前一个节点
		update[i] = x;
	}
	// 此处假设插入节点的成员对象不存在于当前跳跃表内，即不存在重复的节点
	// 随机生成一个level值
	//level = zslRandomLevel();
	level = ybx_zslRandomLevel();//!!!!!!!!!!!!!!!
	if (level > zsl->level) {
		// 如果level大于当前存储的最大level值
		// 设定rank数组中大于原level层以上的值为0
		// 同时设定update数组大于原level层以上的数据
		for (i = zsl->level; i < level; i++) {
			rank[i] = 0;
			update[i] = zsl->header;
			update[i]->level[i].span = zsl->length;
		}
		// 更新level值
		zsl->level = level;
	}
	// 创建插入节点
	x = zslCreateNode(level,score,obj);
	for (i = 0; i < level; i++) {
		// 针对跳跃表的每一层，改变其forward指针的指向
		x->level[i].forward = update[i]->level[i].forward;
		update[i]->level[i].forward = x;
		// 更新插入节点的span值
		x->level[i].span = update[i]->level[i].span - (rank[0] - rank[i]);
		// 更新插入点的前一个节点的span值
		update[i]->level[i].span = (rank[0] - rank[i]) + 1;
	}
	// 更新高层的span值
	for (i = level; i < zsl->level; i++) {
		update[i]->level[i].span++;
	}
	// 设定插入节点的backward指针
	x->backward = (update[0] == zsl->header) ? NULL : update[0];
	if (x->level[0].forward)
		x->level[0].forward->backward = x;
	else
		zsl->tail = x;
	// 跳跃表长度+1
	zsl->length++;
	return x;
}


/* Internal function used by zslDelete, zslDeleteByScore and zslDeleteByRank */
void zslDeleteNode(zskiplist *zsl, zskiplistNode *x, zskiplistNode **update) {
    int i;
    for (i = 0; i < zsl->level; i++) {
        if (update[i]->level[i].forward == x) {
            update[i]->level[i].span += x->level[i].span - 1;
            update[i]->level[i].forward = x->level[i].forward;
        } else {
            update[i]->level[i].span -= 1;
        }
    }
    if (x->level[0].forward) {
        x->level[0].forward->backward = x->backward;
    } else {
        zsl->tail = x->backward;
    }
    while(zsl->level > 1 && zsl->header->level[zsl->level-1].forward == NULL)
        zsl->level--;
    zsl->length--;
}

/* Delete an element with matching score/object from the skiplist. */
int zslDelete(zskiplist *zsl, double score, robj *obj) {
    zskiplistNode *update[ZSKIPLIST_MAXLEVEL], *x;
    int i;

    x = zsl->header;
    for (i = zsl->level-1; i >= 0; i--) {
        while (x->level[i].forward &&
            (x->level[i].forward->score < score ||
                (x->level[i].forward->score == score &&
                compareStringObjects(x->level[i].forward->obj,obj) < 0)))
            x = x->level[i].forward;
        update[i] = x;
    }
    /* We may have multiple elements with the same score, what we need
     * is to find the element with both the right score and object. */
    x = x->level[0].forward;
    if (x && score == x->score && equalStringObjects(x->obj,obj)) {
        zslDeleteNode(zsl, x, update);
        zslFreeNode(x);
        return 1;
    }
    return 0; /* not found */
}

int zslValueGteMin(double value, zrangespec *spec) {
    return spec->minex ? (value > spec->min) : (value >= spec->min);
}

int zslValueLteMax(double value, zrangespec *spec) {
    return spec->maxex ? (value < spec->max) : (value <= spec->max);
}

/* Returns if there is a part of the zset is in range. */
int zslIsInRange(zskiplist *zsl, zrangespec *range) {
    zskiplistNode *x;

    /* Test for ranges that will always be empty. */
    if (range->min > range->max ||
            (range->min == range->max && (range->minex || range->maxex)))
        return 0;
    x = zsl->tail;
    if (x == NULL || !zslValueGteMin(x->score,range))
        return 0;
    x = zsl->header->level[0].forward;
    if (x == NULL || !zslValueLteMax(x->score,range))
        return 0;
    return 1;
}

/* Find the first node that is contained in the specified range.
 * Returns NULL when no element is contained in the range. */
zskiplistNode *zslFirstInRange(zskiplist *zsl, zrangespec *range) {
    zskiplistNode *x;
    int i;

    /* If everything is out of range, return early. */
    if (!zslIsInRange(zsl,range)) return NULL;

    x = zsl->header;
    for (i = zsl->level-1; i >= 0; i--) {
        /* Go forward while *OUT* of range. */
        while (x->level[i].forward &&
            !zslValueGteMin(x->level[i].forward->score,range))
                x = x->level[i].forward;
    }

    /* This is an inner range, so the next node cannot be NULL. */
    x = x->level[0].forward;
    serverAssert(x != NULL);


    /* Check if score <= max. */
    if (!zslValueLteMax(x->score,range)) return NULL;
    return x;
}

/* Find the rank for an element by both score and key.
 * Returns 0 when the element cannot be found, rank otherwise.
 * Note that the rank is 1-based due to the span of zsl->header to the
 * first element. */
unsigned long zslGetRank(zskiplist *zsl, double score, robj *o) {
    zskiplistNode *x;
    unsigned long rank = 0;
    int i;

    x = zsl->header;
    for (i = zsl->level-1; i >= 0; i--) {
        while (x->level[i].forward &&
            (x->level[i].forward->score < score ||
                (x->level[i].forward->score == score &&
                compareStringObjects(x->level[i].forward->obj,o) <= 0))) {
            rank += x->level[i].span;
            x = x->level[i].forward;
        }

        /* x might be equal to zsl->header, so test if obj is non-NULL */
        if (x->obj && equalStringObjects(x->obj,o)) {
            return rank;
        }
    }
    return 0;
}

/* Finds an element by its rank. The rank argument needs to be 1-based. */
zskiplistNode* zslGetElementByRank(zskiplist *zsl, unsigned long rank) {//返回排位为rank的节点地址
    zskiplistNode *x;
    unsigned long traversed = 0;//排位值，跨越过的节点数
    int i;

    x = zsl->header;
    for (i = zsl->level-1; i >= 0; i--) {//遍历头结点的每一层
        while (x->level[i].forward && (traversed + x->level[i].span) <= rank)//知道traversed还没到到达rank
        {
            traversed += x->level[i].span;//每次跟新排位值
            x = x->level[i].forward;//指向下一个节点
        }
        if (traversed == rank) { //跨越的节点数等于排位值，返回节点地址
            return x;
        }
    }
    return NULL;
}

/*-----------------------------------------------------------------------------
 * Ziplist-backed sorted set API
 * 压缩列表实现的有序列表API
 *----------------------------------------------------------------------------*/
// 从sptr指向的entry中取出有序集合的分值
double zzlGetScore(unsigned char *sptr) {
    unsigned char *vstr;
    unsigned int vlen;
    long long vlong;
    char buf[128];
    double score;

    serverAssert(sptr != NULL);
    // 将sptr指向的entry保存到参数中
    serverAssert(ziplistGet(sptr,&vstr,&vlen,&vlong));

    // 如果是字符串类型的值
    if (vstr) {
        // 拷贝到buf中
        memcpy(buf,vstr,vlen);
        buf[vlen] = '\0';
        // 转换成double类型
        score = strtod(buf,NULL);
    // 整数值
    } else {
        score = vlong;
    }

    return score; //返回分值
}//end of zzlGetScore()

/* Compare element in sorted set with given element. */
int zzlCompareElements(unsigned char *eptr, unsigned char *cstr, unsigned int clen) {
    unsigned char *vstr;
    unsigned int vlen;
    long long vlong;
    unsigned char vbuf[32];
    int minlen, cmp;

    serverAssert(ziplistGet(eptr,&vstr,&vlen,&vlong));
    if (vstr == NULL) {
        /* Store string representation of long long in buf. */
        vlen = ll2string((char*)vbuf,sizeof(vbuf),vlong);
        vstr = vbuf;
    }

    minlen = (vlen < clen) ? vlen : clen;
    cmp = memcmp(vstr,cstr,minlen);
    if (cmp == 0) return vlen-clen;
    return cmp;
}//end of zzlCompareElements()


unsigned int zzlLength(unsigned char *zl) {
    return ziplistLen(zl)/2;
}



//将ele元素和分值score插入到eptr指向节点的前面
unsigned char *zzlInsertAt(unsigned char *zl, unsigned char *eptr, robj *ele, double score) {
    unsigned char *sptr;
    char scorebuf[128];
    int scorelen;
    size_t offset;

    serverAssertWithInfo(NULL,ele,sdsEncodedObject(ele));
    // 讲score转换为字符串，保存到scorebuf中，并计算score的字节长度
    scorelen = d2string(scorebuf,sizeof(scorebuf),score);

    // 如果eptr为空，则加到ziplist的尾部
    if (eptr == NULL) {
        zl = ziplistPush(zl,ele->ptr,sdslen(ele->ptr),ZIPLIST_TAIL);    //添加元素节点
        zl = ziplistPush(zl,(unsigned char*)scorebuf,scorelen,ZIPLIST_TAIL);    //添加分值

    // 插入在eptr节点的前面
    } else {
        /* Keep offset relative to zl, as it might be re-allocated. */
        // 备份当eptr指向节点的偏移量，防止内存被重新分配
        offset = eptr-zl;
        // 将元素插入在eptr前面
        zl = ziplistInsert(zl,eptr,ele->ptr,sdslen(ele->ptr));
        eptr = zl+offset;

        /* Insert score after the element. */
        // 找到分值节点的地址
        serverAssertWithInfo(NULL,ele,(sptr = ziplistNext(zl,eptr)) != NULL);
        // 插入分值
        zl = ziplistInsert(zl,sptr,(unsigned char*)scorebuf,scorelen);
    }

    return zl;
}//end of zzlInsertAt()


/* Insert (element,score) pair in ziplist. This function assumes the element is
 * not yet present in the list. */
unsigned char *zzlInsert(unsigned char *zl, robj *ele, double score) {
    unsigned char *eptr = ziplistIndex(zl,0), *sptr;
    double s;

    ele = getDecodedObject(ele);
    while (eptr != NULL) {
        sptr = ziplistNext(zl,eptr);
        serverAssertWithInfo(NULL,ele,sptr != NULL);
        s = zzlGetScore(sptr);

        if (s > score) {
            /* First element with score larger than score for element to be
             * inserted. This means we should take its spot in the list to
             * maintain ordering. */
            zl = zzlInsertAt(zl,eptr,ele,score);
            break;
        } else if (s == score) {
            /* Ensure lexicographical ordering for elements. */
            if (zzlCompareElements(eptr,ele->ptr,sdslen(ele->ptr)) > 0) {
                zl = zzlInsertAt(zl,eptr,ele,score);
                break;
            }
        }

        /* Move to next element. */
        eptr = ziplistNext(zl,sptr);
    }

    /* Push on tail of list when it was not yet inserted. */
    if (eptr == NULL)
        zl = zzlInsertAt(zl,NULL,ele,score);

    decrRefCount(ele);
    return zl;
}




/*-----------------------------------------------------------------------------
 * Common sorted set API
 *----------------------------------------------------------------------------*/

unsigned int zsetLength(robj *zobj) {
    int length = -1;
    if (zobj->encoding == OBJ_ENCODING_ZIPLIST) {
        length = zzlLength(zobj->ptr);
    } else if (zobj->encoding == OBJ_ENCODING_SKIPLIST) {
        length = ((zset*)zobj->ptr)->zsl->length;
    } else {
        serverPanic("Unknown sorted set encoding");
    }
    return length;
}



// 将有序集合对象的编码转换为encoding制定的编码类型
void zsetConvert(robj *zobj, int encoding) {
    zset *zs;
    zskiplistNode *node, *next;
    robj *ele;
    double score;

    if (zobj->encoding == encoding) return;
    // 从ziplist转换到skiplist
    if (zobj->encoding == OBJ_ENCODING_ZIPLIST) {
        unsigned char *zl = zobj->ptr;
        unsigned char *eptr, *sptr;
        unsigned char *vstr;
        unsigned int vlen;
        long long vlong;

        if (encoding != OBJ_ENCODING_SKIPLIST)
            serverPanic("Unknown target encoding");

        // 分配有序集合空间
        zs = zmalloc(sizeof(*zs));
        // 初始化字典和跳跃表成员
        zs->dict = dictCreate(&zsetDictType,NULL);
        zs->zsl = zslCreate();

        // 压缩列表的首元素指针
        eptr = ziplistIndex(zl,0);
        serverAssertWithInfo(NULL,zobj,eptr != NULL);
        // 压缩列表的首元素分值指针
        sptr = ziplistNext(zl,eptr);
        serverAssertWithInfo(NULL,zobj,sptr != NULL);

        // 遍历压缩列表的entry
        while (eptr != NULL) {
            score = zzlGetScore(sptr);  //取出分值
            serverAssertWithInfo(NULL,zobj,ziplistGet(eptr,&vstr,&vlen,&vlong));    //取出元素
            // 根据字符串或整数创建字符串类型对象
            if (vstr == NULL)
                ele = createStringObjectFromLongLong(vlong);
            else
                ele = createStringObject((char*)vstr,vlen);

            /* Has incremented refcount since it was just created. */
            // 将分值和成员插入到跳跃表中
            node = zslInsert(zs->zsl,score,ele);
            // 将分值和成员添加到字典中
            serverAssertWithInfo(NULL,zobj,dictAdd(zs->dict,ele,&node->score) == DICT_OK);
            incrRefCount(ele); /* Added to dictionary. */
            zzlNext(zl,&eptr,&sptr);    //更新元素和分值的指针
        }

        // 转换到skiplist编码后，将原来ziplist的空间释放
        zfree(zobj->ptr);
        // 设置编码和有序集合的值
        zobj->ptr = zs;
        zobj->encoding = OBJ_ENCODING_SKIPLIST;

    // 从OBJ_ENCODING_SKIPLIST转换到OBJ_ENCODING_ZIPLIST
    } else if (zobj->encoding == OBJ_ENCODING_SKIPLIST) {
        unsigned char *zl = ziplistNew();   //创建新的压缩列表

        if (encoding != OBJ_ENCODING_ZIPLIST)
            serverPanic("Unknown target encoding");

        /* Approach similar to zslFree(), since we want to free the skiplist at
         * the same time as creating the ziplist. */
        // 指向有序集合的地址
        zs = zobj->ptr;
        dictRelease(zs->dict);  //释放有序集合的字典，根据跳跃表遍历即可
        // 指向跳跃表的首节点
        node = zs->zsl->header->level[0].forward;
        // 释放表头节点和表头空间
        zfree(zs->zsl->header);
        zfree(zs->zsl);

        //遍历跳跃表的节点
        while (node) {
            // 将节点对象解码成字符串对象
            ele = getDecodedObject(node->obj);
            // 将当前元素和分值尾插到列表中
            zl = zzlInsertAt(zl,NULL,ele,node->score);
            decrRefCount(ele);

            // 备份下一个节点地址
            next = node->level[0].forward;
            // 释放当前节点空间
            zslFreeNode(node);
            // 指向下一个节点
            node = next;
        }

        zfree(zs);  //释放原来skiplist编码的有序集合
        // 设置新的有序集合和编码
        zobj->ptr = zl;
        zobj->encoding = OBJ_ENCODING_ZIPLIST;
    } else {
        serverPanic("Unknown sorted set encoding");
    }
}




/* Move to next entry based on the values in eptr and sptr. Both are set to
 * NULL when there is no next entry. */
// 将当前的元素指针eptr和当前元素分值的指针sptr都指向下一个元素和元素的分值
void zzlNext(unsigned char *zl, unsigned char **eptr, unsigned char **sptr) {
    unsigned char *_eptr, *_sptr;
    serverAssert(*eptr != NULL && *sptr != NULL);

    // 下一个元素的地址为当前元素分值的下一个entry
    _eptr = ziplistNext(zl,*sptr);
    // 下一个entry不为空
    if (_eptr != NULL) {
        // 下一个元素分值的地址为下一个元素的下一个entry
        _sptr = ziplistNext(zl,_eptr);
        serverAssert(_sptr != NULL);
    } else {
        /* No next entry. */
        _sptr = NULL;
    }

    //保存到参数中返回
    *eptr = _eptr;
    *sptr = _sptr;
}

