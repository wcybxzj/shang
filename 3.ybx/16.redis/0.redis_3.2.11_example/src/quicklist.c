#include <string.h> /* for memcpy */
#include "quicklist.h"
#include "zmalloc.h"
#include "ziplist.h"
#include "util.h" /* for ll2string */
#include "lzf.h"

#if defined(REDIS_TEST) || defined(REDIS_TEST_VERBOSE)
#include <stdio.h> /* for printf (debug printing), snprintf (genstr) */
#endif

#ifndef REDIS_STATIC
#define REDIS_STATIC static
#endif

/* Optimization levels for size-based filling */
static const size_t optimization_level[] = {4096, 8192, 16384, 32768, 65536};

/* Maximum size in bytes of any multi-element ziplist.
 * Larger values will live in their own isolated ziplists. */
#define SIZE_SAFETY_LIMIT 8192

/* Minimum ziplist size in bytes for attempting compression. */
#define MIN_COMPRESS_BYTES 48

/* Minimum size reduction in bytes to store compressed quicklistNode data.
 * This also prevents us from storing compression if the compression
 * resulted in a larger size than the original data. */
#define MIN_COMPRESS_IMPROVE 8

/* If not verbose testing, remove all debug printing. */
#ifndef REDIS_TEST_VERBOSE
#define D(...)
#else
#define D(...)                                                                 \
    do {                                                                       \
        printf("%s:%s:%d:\t", __FILE__, __FUNCTION__, __LINE__);               \
        printf(__VA_ARGS__);                                                   \
        printf("\n");                                                          \
    } while (0);
#endif

/* Simple way to give quicklistEntry structs default values with one call. */
#define initEntry(e)                                                           \
    do {                                                                       \
        (e)->zi = (e)->value = NULL;                                           \
        (e)->longval = -123456789;                                             \
        (e)->quicklist = NULL;                                                 \
        (e)->node = NULL;                                                      \
        (e)->offset = 123456789;                                               \
        (e)->sz = 0;                                                           \
    } while (0)

#if __GNUC__ >= 3
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

/* Create a new quicklist.
 * Free with quicklistRelease(). */
quicklist *quicklistCreate(void) {      //创建一个新的quicklist，并初始化成员
    struct quicklist *quicklist;
    quicklist = zmalloc(sizeof(*quicklist));    //分配空间
    //初始化各个成员
    quicklist->head = quicklist->tail = NULL;
    quicklist->len = 0;
    quicklist->count = 0;
    quicklist->compress = 0;                    //默认不压缩
    quicklist->fill = -2;                       //设置默认值，每个ziplist的字节数最大为8kb
    return quicklist;
}

#define COMPRESS_MAX (1 << 16)  //最大压缩程度为2^16，因为compress长度为16位
void quicklistSetCompressDepth(quicklist *quicklist, int compress) {    //设置压缩程度
    if (compress > COMPRESS_MAX) {
        compress = COMPRESS_MAX;
    } else if (compress < 0) {      //小于0也不压缩
        compress = 0;
    }
    quicklist->compress = compress;
}

#define FILL_MAX (1 << 15)      //每个ziplist最多有2^15个entry节点
void quicklistSetFill(quicklist *quicklist, int fill) {         //设置ziplist结构的大小
    if (fill > FILL_MAX) {
        fill = FILL_MAX;
    } else if (fill < -5) { //ziplist字节数最大可以为64kb
        fill = -5;
    }
    quicklist->fill = fill;
}

//设置压缩列表表头的fill和compress成员
void quicklistSetOptions(quicklist *quicklist, int fill, int depth) {
    quicklistSetFill(quicklist, fill);
    quicklistSetCompressDepth(quicklist, depth);
}

/* Create a new quicklist with some default parameters. */
//创建一个quicklist，并且设置默认的参数
quicklist *quicklistNew(int fill, int compress) {
    quicklist *quicklist = quicklistCreate();
    quicklistSetOptions(quicklist, fill, compress);
    return quicklist;
}

//创建一个quicklist节点quicklistNode，并初始化
REDIS_STATIC quicklistNode *quicklistCreateNode(void) {
    quicklistNode *node;
    node = zmalloc(sizeof(*node));
    node->zl = NULL;
    node->count = 0;
    node->sz = 0;
    node->next = node->prev = NULL;
    node->encoding = QUICKLIST_NODE_ENCODING_RAW;           //默认不压缩
    node->container = QUICKLIST_NODE_CONTAINER_ZIPLIST;     //默认使用ziplist结构存储数据
    node->recompress = 0;                                   //设置没压缩的标志
    return node;
}

/* Return cached quicklist count */
unsigned int quicklistCount(quicklist *ql) { return ql->count; }

/* Free entire quicklist. */
//释放整个quicklist
void quicklistRelease(quicklist *quicklist) {
	unsigned long len;
	quicklistNode *current, *next;

	current = quicklist->head;
	len = quicklist->len;       //记录quicklistNode节点个数
	while (len--) {
		next = current->next;   //备份后继节点地址

		zfree(current->zl);     //释放当前节点的ziplist结构
		quicklist->count -= current->count; //从quicklist的entry计数器减取当前节点中的entry个数

		zfree(current);         //释放当前quicklistNode节点

		quicklist->len--;       //更新quicklistNode节点计数器
		current = next;         //指向后继节点
	}
	zfree(quicklist);           //释放整个quicklist
}


/* Compress the ziplist in 'node' and update encoding details.
 * Returns 1 if ziplist compressed successfully.
 * Returns 0 if compression failed or if ziplist too small to compress. */
//压缩node节点,返回0表示压缩失败，1表示成功
REDIS_STATIC int __quicklistCompressNode(quicklistNode *node) {
#ifdef REDIS_TEST
	node->attempted_compress = 1;           //测试标志
#endif

	/* Don't bother compressing small values */
	if (node->sz < MIN_COMPRESS_BYTES)  //如果ziplist大小小于48字节，压缩失败，返回0
		return 0;

	quicklistLZF *lzf = zmalloc(sizeof(*lzf) + node->sz);   //分配空间

	/* Cancel if compression fails or doesn't compress small enough */
	//调用laf压缩函数进行压缩并返回压缩后的大小
	if (((lzf->sz = lzf_compress(node->zl, node->sz, lzf->compressed,
						node->sz)) == 0) ||
			lzf->sz + MIN_COMPRESS_IMPROVE >= node->sz) {
		/* lzf_compress aborts/rejects compression if value not compressable. */
		zfree(lzf); //太小不能压缩或压缩失败则释放空间，返回0
		return 0;
	}
	//压缩成功并分配压缩成功大小的空间
	lzf = zrealloc(lzf, sizeof(*lzf) + lzf->sz);
	zfree(node->zl);    //释放原来的空间
	node->zl = (unsigned char *)lzf;    //设置zl指向quicklistLZF结构
	node->encoding = QUICKLIST_NODE_ENCODING_LZF;   //设置encoding编码为lzf类型
	node->recompress = 0;       //不需要被再次压缩
	return 1;   //压缩成功返回1
}

/* Compress only uncompressed nodes. */
//只压缩之前没有压缩过的节点
#define quicklistCompressNode(_node)                                           \
	do {                                                                       \
		if ((_node) && (_node)->encoding == QUICKLIST_NODE_ENCODING_RAW) {     \
			__quicklistCompressNode((_node));                                  \
		}                                                                      \
	} while (0)


/* Uncompress the ziplist in 'node' and update encoding details.
 * Returns 1 on successful decode, 0 on failure to decode. */
//解压缩node节点的zl，成功返回1 ，失败返回0
REDIS_STATIC int __quicklistDecompressNode(quicklistNode *node) {
#ifdef REDIS_TEST
	node->attempted_compress = 0;
#endif

	void *decompressed = zmalloc(node->sz); //分配存储空间
	quicklistLZF *lzf = (quicklistLZF *)node->zl;

	//调用lzf_decompress进行解压缩
	if (lzf_decompress(lzf->compressed, lzf->sz, decompressed, node->sz) == 0) {
		/* Someone requested decompress, but we can't decompress.  Not good. */
		zfree(decompressed);    //解压缩失败要释放之前分配的空间
		return 0;               //解压缩失败
	}
	zfree(lzf);                 //释放之前的压缩过的空间
	node->zl = decompressed;    //指向解压缩出来的空间
	node->encoding = QUICKLIST_NODE_ENCODING_RAW;   //设置为没压缩的标志，成功返回1
	return 1;
}

/* Decompress only compressed nodes. */
//解压缩节点_node，_node必须是压缩过的节点
#define quicklistDecompressNode(_node)                                         \
	do {                                                                       \
		if ((_node) && (_node)->encoding == QUICKLIST_NODE_ENCODING_LZF) {     \
			__quicklistDecompressNode((_node));                                \
		}                                                                      \
	} while (0)

/* Force node to not be immediately re-compresable */
//标记被解压的_node节点已经被解压，等待被再次压缩
#define quicklistDecompressNodeForUse(_node)                                   \
    do {                                                                       \
        if ((_node) && (_node)->encoding == QUICKLIST_NODE_ENCODING_LZF) {     \
            __quicklistDecompressNode((_node));                                \
            (_node)->recompress = 1;                                           \
        }                                                                      \
    } while (0)

//移到quicklist.h
//返回1表示可以压缩，返回0表示不可以压缩
//#define quicklistAllowsCompression(_ql) ((_ql)->compress != 0)

size_t quicklistGetLzf(const quicklistNode *node, void **data) {
    quicklistLZF *lzf = (quicklistLZF *)node->zl;
    *data = lzf->compressed;
    return lzf->sz;
}

//如果node不在压缩程度的范围内，就压缩
REDIS_STATIC void __quicklistCompress(const quicklist *quicklist,
		quicklistNode *node) {
	/* If length is less than our compress depth (from both sides),
	 * we can't compress anything. */
	//如果quicklist不能压缩或者压缩程度太大直接返回
	//quicklist->compress * 2 就是压缩节点的个数，不能比总结的的个数len还要多
	if (!quicklistAllowsCompression(quicklist) ||
			quicklist->len < (unsigned int)(quicklist->compress * 2))
		return;

	//压缩：压缩中间的节点，所以从两边开始忘中间找节点
	//记录quicklistNode头部节点和尾部节点的地址
	quicklistNode *forward = quicklist->head;
	quicklistNode *reverse = quicklist->tail;
	int depth = 0;
	int in_depth = 0;

	//从两边往中间找，找compress次
	while (depth++ < quicklist->compress) {
		//如果压缩过，则将其解压缩
		quicklistDecompressNode(forward);
		quicklistDecompressNode(reverse);

		//如果找到node节点设置标记
		if (forward == node || reverse == node)
			in_depth = 1;

		//如果两个指针相遇，返回
		if (forward == reverse)
			return;

		//更新指针，指向下一个节点
		forward = forward->next;
		reverse = reverse->prev;
	}

	//如果node不在两边不需要压缩的范围内，则要压缩这个节点
	if (!in_depth)
		quicklistCompressNode(node);

	if (depth > 2) {    //压缩两个指针指向的节点
		/* At this point, forward and reverse are one node beyond depth */
		quicklistCompressNode(forward);
		quicklistCompressNode(reverse);
	}
}// end of __quicklistCompress()

//如果_node可以压缩，则要压缩
#define quicklistCompress(_ql, _node)                                          \
	do {                                                                       \
		/*如果_node节点需要被再次压缩，则压缩_node节点*/                        \
		if ((_node)->recompress)                                               \
		quicklistCompressNode((_node));                                    \
		else                                                                   \
		__quicklistCompress((_ql), (_node));                               \
		/*否则，进行范围压缩*/                                                  \
	} while (0)


/* Insert 'new_node' after 'old_node' if 'after' is 1.
 * Insert 'new_node' before 'old_node' if 'after' is 0.
 * Note: 'new_node' is *always* uncompressed, so if we assign it to
 *       head or tail, we do not need to uncompress it. */
//插入节点，如果after为1，则new_node插在old_node后面，如果为0，则插在前面
//插入的new_node总是未压缩的，所以可以插在头部或尾部。
REDIS_STATIC void __quicklistInsertNode(quicklist *quicklist,
		quicklistNode *old_node,
		quicklistNode *new_node, int after) {
	if (after) {    //插入在后面
		new_node->prev = old_node;                  //new_node的前驱指针指向old_node
		if (old_node) {                             //如果old_node非空
			new_node->next = old_node->next;        //new_node插在old_new的后面
			if (old_node->next)
				old_node->next->prev = new_node;    //old_node的后驱节点的前驱指针变成new_node
			old_node->next = new_node;              //old_node的后驱节点更新为new_node
		}
		if (quicklist->tail == old_node)            //如果old_node节点是尾节点，需要更新尾节点指针的指向
			quicklist->tail = new_node;
	} else {        //插入在前面
		new_node->next = old_node;                  //将old_node插在new_node后面
		if (old_node) {
			new_node->prev = old_node->prev;        //更新new_node的前驱指针为old_node的前驱指针
			if (old_node->prev)
				old_node->prev->next = new_node;    //更新old_node的前驱节点的后继指针为new_node
			old_node->prev = new_node;              //将old_node的前驱指针指向new_node
		}
		if (quicklist->head == old_node)            //如果old_node节点是头节点，需要更新头节点指针的指向
			quicklist->head = new_node;
	}
	/* If this insert creates the only element so far, initialize head/tail. */
	if (quicklist->len == 0) {                      //如果quicklist为空列表
		quicklist->head = quicklist->tail = new_node;//更新头尾节点指针
	}

	if (old_node)
		quicklistCompress(quicklist, old_node);     //如果设置了compress，压缩old_node

	quicklist->len++;                               //更新quicklistNode节点计数器
}

/* Wrappers for node inserting around existing node. */
//将前插法封装封装起来
REDIS_STATIC void _quicklistInsertNodeBefore(quicklist *quicklist,
		quicklistNode *old_node,
		quicklistNode *new_node) {
	__quicklistInsertNode(quicklist, old_node, new_node, 0);
}

//将后插法封装封装起来
REDIS_STATIC void _quicklistInsertNodeAfter(quicklist *quicklist,
		quicklistNode *old_node,
		quicklistNode *new_node) {
	__quicklistInsertNode(quicklist, old_node, new_node, 1);
}


//sz是否满足小于fill所要求的最大值
REDIS_STATIC int
_quicklistNodeSizeMeetsOptimizationRequirement(const size_t sz,
		const int fill) {
	if (fill >= 0)
		return 0;

	//fill小于0
	size_t offset = (-fill) - 1;    //数组的偏移量

	//static const size_t optimization_level[] = {4096, 8192, 16384, 32768, 65536};
	//(sizeof(optimization_level) / sizeof(*optimization_level) 求出数组的元素个数
	//数组偏移量要小于数组元素
	if (offset < (sizeof(optimization_level) / sizeof(*optimization_level))) {
		if (sz <= optimization_level[offset]) {     //sz小于根据fill算出的等级，返回1
			return 1;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

//sz是否超过ziplist所规定的安全界限8192字节，1表示安全，0表示不安全
#define sizeMeetsSafetyLimit(sz) ((sz) <= SIZE_SAFETY_LIMIT)

//node节点中ziplist能否插入entry节点中，根据fill和sz判断
REDIS_STATIC int _quicklistNodeAllowInsert(const quicklistNode *node,
		const int fill, const size_t sz) {
	if (unlikely(!node))    //!node为假的可能性大，如果Node为空直接返回
		return 0;

	int ziplist_overhead;
	/* size of previous offset */
	//上一个entry节点的信息
	if (sz < 254)                   //如果ziplist的大小小于254
		ziplist_overhead = 1;       //编码需要1个字节
	else
		ziplist_overhead = 5;       //否则需要5个字节

	/* size of forward offset */
	//当前entry节点的信息
	if (sz < 64)                    //长度小于2^6-1 用1个字节编码
		ziplist_overhead += 1;
	else if (likely(sz < 16384))    //长度小于2^14-1 用2个字节编码
		ziplist_overhead += 2;
	else
		ziplist_overhead += 5;      //长度小于2^32-1 用5个字节编码

	/* new_sz overestimates if 'sz' encodes to an integer type */
	unsigned int new_sz = node->sz + sz + ziplist_overhead;
	if (likely(_quicklistNodeSizeMeetsOptimizationRequirement(new_sz, fill)))   //new_sz符合fill配置，成功
		return 1;
	else if (!sizeMeetsSafetyLimit(new_sz)) //new_sz不满足安全界限，返回0
		return 0;
	else if ((int)node->count < fill)   //如果entry节点个数小于fill所设置的大小(正数情况)，返回1
		return 1;
	else
		return 0;
}


//更新Node的ziplist大小sz，将zl的zlbytes成员赋值给sz
#define quicklistNodeUpdateSz(node)                                            \
	do {                                                                       \
		(node)->sz = ziplistBlobLen((node)->zl);                               \
	} while (0)     //ziplistBlobLen返回整个 ziplist 占用的内存字节数




/* Add new entry to head node of quicklist.
 *
 * Returns 0 if used existing head.
 * Returns 1 if new head created. */
//push一个entry节点到quicklist的头部
//返回0表示不改变头节点指针，返回1表示节点插入在头部，改变了头结点指针
int quicklistPushHead(quicklist *quicklist, void *value, size_t sz) {
	quicklistNode *orig_head = quicklist->head; //备份头结点地址

	//如果ziplist可以插入entry节点
	if (likely(
				_quicklistNodeAllowInsert(quicklist->head, quicklist->fill, sz))) {
		quicklist->head->zl =
			ziplistPush(quicklist->head->zl, value, sz, ZIPLIST_HEAD);  //将节点push到头部
		quicklistNodeUpdateSz(quicklist->head); //更新quicklistNode记录ziplist大小的sz
	} else {        //如果不能插入entry节点到ziplist
		quicklistNode *node = quicklistCreateNode();    //新创建一个quicklistNode节点

		//将entry节点push到新创建的quicklistNode节点中
		node->zl = ziplistPush(ziplistNew(), value, sz, ZIPLIST_HEAD);

		quicklistNodeUpdateSz(node);    //更新ziplist的大小sz
		_quicklistInsertNodeBefore(quicklist, quicklist->head, node);   //将新创建的节点插入到头节点前
	}
	quicklist->count++;                     //更新quicklistNode计数器
	quicklist->head->count++;               //更新entry计数器
	return (orig_head != quicklist->head);  //如果改变头节点指针则返回1，否则返回0
}

/* Add new entry to tail node of quicklist.
 *
 * Returns 0 if used existing tail.
 * Returns 1 if new tail created. */
//push一个entry节点到quicklist的尾节点中，如果不能push则新创建一个quicklistNode节点
//返回0表示不改变尾节点指针，返回1表示节点插入在尾部，改变了尾结点指针
int quicklistPushTail(quicklist *quicklist, void *value, size_t sz) {
	quicklistNode *orig_tail = quicklist->tail;
	//如果ziplist可以插入entry节点
	if (likely(
				_quicklistNodeAllowInsert(quicklist->tail, quicklist->fill, sz))) {
		quicklist->tail->zl =
			ziplistPush(quicklist->tail->zl, value, sz, ZIPLIST_TAIL);  //将节点push到尾部
		quicklistNodeUpdateSz(quicklist->tail); //更新quicklistNode记录ziplist大小的sz
	} else {
		quicklistNode *node = quicklistCreateNode();        //新创建一个quicklistNode节点

		//将entry节点push到新创建的quicklistNode节点中
		node->zl = ziplistPush(ziplistNew(), value, sz, ZIPLIST_TAIL);

		quicklistNodeUpdateSz(node);        //更新ziplist的大小sz
		_quicklistInsertNodeAfter(quicklist, quicklist->tail, node);//将新创建的节点插入到尾节点后
	}
	quicklist->count++;             //更新quicklistNode计数器
	quicklist->tail->count++;       //更新entry计数器
	return (orig_tail != quicklist->tail);  //如果改变尾节点指针则返回1，否则返回0
}


//删除quicklistNode节点node
REDIS_STATIC void __quicklistDelNode(quicklist *quicklist,
                                     quicklistNode *node) {
    if (node->next) //如果后继节点指针不为空，则将后继节点的前驱指针跳过当前节点
        node->next->prev = node->prev;
    if (node->prev) //如果前驱节点指针不为空，则将前驱节点的后继指针跳过当前节点
        node->prev->next = node->next;

    if (node == quicklist->tail) {  //如果被删除的节点是尾节点，则要更新尾节点指针
        quicklist->tail = node->prev;
    }

    if (node == quicklist->head) {  //如果被删除的节点是头节点，则要更新头节点指针
        quicklist->head = node->next;
    }

    /* If we deleted a node within our compress depth, we
     * now have compressed nodes needing to be decompressed. */
    __quicklistCompress(quicklist, NULL);   //如果删除的节点在压缩的范围内，必须更新quicklist的压缩情况

    quicklist->count -= node->count;    //更新entry计数器

    zfree(node->zl);                    //释放空间
    zfree(node);
    quicklist->len--;                   //更新quicklistNode计数器
}

/* Delete one entry from list given the node for the entry and a pointer
 * to the entry in the node.
 *
 * Note: quicklistDelIndex() *requires* uncompressed nodes because you
 *       already had to get *p from an uncompressed node somewhere.
 *
 * Returns 1 if the entire node was deleted, 0 if node still exists.
 * Also updates in/out param 'p' with the next offset in the ziplist. */
//删除ziplist中的entry，如果entry是最后一个，则删除当前quicklistNode节点，返回1，没有删除当前节点则返回0
REDIS_STATIC int quicklistDelIndex(quicklist *quicklist, quicklistNode *node,
                                   unsigned char **p) {
    int gone = 0;

    node->zl = ziplistDelete(node->zl, p);  //删除p指向的entry
    node->count--;                          //更新计数器
    if (node->count == 0) {                 //如果entry为0
        gone = 1;
        __quicklistDelNode(quicklist, node);//删除当前的quicklistNode节点
    } else {
        quicklistNodeUpdateSz(node);        //否则更新node中ziplist大小sz
    }
    quicklist->count--;                     //更新quicklist表头中的quicklistNode节点计数器
    /* If we deleted the node, the original node is no longer valid */
    return gone ? 1 : 0;
}

/* pop from quicklist and return result in 'data' ptr.  Value of 'data'
 * is the return value of 'saver' function pointer if the data is NOT a number.
 *
 * If the quicklist element is a long long, then the return value is returned in
 * 'sval'.
 *
 * Return value of 0 means no elements available.
 * Return value of 1 means check 'data' and 'sval' for values.
 * If 'data' is set, use 'data' and 'sz'.  Otherwise, use 'sval'. */
//从quicklist的头节点或尾节点pop弹出出一个entry，并将value保存在传入传出参数
//返回0表示没有可pop出的entry
//返回1表示pop出了entry，存在data或sval中
int quicklistPopCustom(quicklist *quicklist, int where, unsigned char **data,
                       unsigned int *sz, long long *sval,
                       void *(*saver)(unsigned char *data, unsigned int sz)) {
    unsigned char *p;
    unsigned char *vstr;
    unsigned int vlen;
    long long vlong;
    int pos = (where == QUICKLIST_HEAD) ? 0 : -1;   //位置下标

    if (quicklist->count == 0)  //entry数量为0，弹出失败
        return 0;

    //初始化
    if (data)
        *data = NULL;
    if (sz)
        *sz = 0;
    if (sval)
        *sval = -123456789;

    quicklistNode *node;
    //记录quicklist的头quicklistNode节点或尾quicklistNode节点
    if (where == QUICKLIST_HEAD && quicklist->head) {
        node = quicklist->head;
    } else if (where == QUICKLIST_TAIL && quicklist->tail) {
        node = quicklist->tail;
    } else {
        return 0;           //只能从头或尾弹出
    }

    p = ziplistIndex(node->zl, pos);    //获得当前pos的entry地址
    if (ziplistGet(p, &vstr, &vlen, &vlong)) {  //将entry信息读入到参数中
        if (vstr) {     //entry中是字符串值
            if (data)
                *data = saver(vstr, vlen);  //调用特定的函数将字符串值保存到*data
            if (sz)
                *sz = vlen;                 //保存字符串长度
        } else {        //整数值
            if (data)
                *data = NULL;
            if (sval)
                *sval = vlong;  //将整数值保存在*sval中
        }
        quicklistDelIndex(quicklist, node, &p); //将该entry从ziplist中删除
        return 1;
    }
    return 0;
}

/* Return a malloc'd copy of data passed in */
//将data内容拷贝一份并返回地址
REDIS_STATIC void *_quicklistSaver(unsigned char *data, unsigned int sz) {
    unsigned char *vstr;
    if (data) {
        vstr = zmalloc(sz);     //分配空间
        memcpy(vstr, data, sz); //拷贝
        return vstr;
    }
    return NULL;
}


/* Default pop function
 *
 * Returns malloc'd value from quicklist */
//pop一个entry值，调用quicklistPopCustom，封装起来
int quicklistPop(quicklist *quicklist, int where, unsigned char **data,
                 unsigned int *sz, long long *slong) {
    unsigned char *vstr;
    unsigned int vlen;
    long long vlong;
    if (quicklist->count == 0)
        return 0;
    int ret = quicklistPopCustom(quicklist, where, &vstr, &vlen, &vlong,
                                 _quicklistSaver);  //pop出一个where位置的entry

    //将pop出的值保存起来
    if (data)
        *data = vstr;
    if (slong)
        *slong = vlong;
    if (sz)
        *sz = vlen;
    return ret;
}

/* Wrapper to allow argument-based switching between HEAD/TAIL pop */
//将push函数封装起来，通过where 表示push头部或push尾部
void quicklistPush(quicklist *quicklist, void *value, const size_t sz, int where) {
	if (where == QUICKLIST_HEAD) {                  //头插
		quicklistPushHead(quicklist, value, sz);
	} else if (where == QUICKLIST_TAIL) {           //尾插
		quicklistPushTail(quicklist, value, sz);
	}
}

//返回当前节点的迭代器的地址
quicklistIter *quicklistGetIterator(const quicklist *quicklist, int direction) {
    quicklistIter *iter;
    iter = zmalloc(sizeof(*iter));  //分配空间

    if (direction == AL_START_HEAD) {   //如果是前向迭代
        iter->current = quicklist->head;    //设置current指向quicklist的头节点
        iter->offset = 0;
    } else if (direction == AL_START_TAIL) {    //如果是反向迭代
        iter->current = quicklist->tail;    //设置current指向quicklist的尾节点
        iter->offset = -1;                  //-1代表反向迭代时尾节点的下标
    }

    //设置成员的初始值
    iter->direction = direction;
    iter->quicklist = quicklist;
    iter->zi = NULL;
    return iter;
}

//释放迭代器
void quicklistReleaseIterator(quicklistIter *iter) {
    if (iter->current)  //如果该迭代器已经和一个quicklistNode结合
        quicklistCompress(iter->quicklist, iter->current);  //按需压缩entry节点

    zfree(iter);
}



/* Get next element in iterator.
 *
 * Note: You must NOT insert into the list while iterating over it.
 * You *may* delete from the list while iterating using the
 * quicklistDelEntry() function.
 * If you insert into the quicklist while iterating, you should
 * re-create the iterator after your addition.
 *
 * iter = quicklistGetIterator(quicklist,<direction>);
 * quicklistEntry entry;
 * while (quicklistNext(iter, &entry)) {
 *     if (entry.value)
 *          [[ use entry.value with entry.sz ]]
 *     else
 *          [[ use entry.longval ]]
 * }
 *
 * Populates 'entry' with values for this iteration.
 * Returns 0 when iteration is complete or if iteration not possible.
 * If return value is 0, the contents of 'entry' are not valid.
 */
//将迭代器当前指向的节点的信息读到quicklistEntry结构中，并且指向下一个节点
int quicklistNext(quicklistIter *iter, quicklistEntry *entry) {
	initEntry(entry);   //初始化

	if (!iter) {
		D("Returning because no iter!");
		return 0;
	}

	entry->quicklist = iter->quicklist; //记录迭代器所属的quicklist
	entry->node = iter->current;        //记录迭代器所属的quicklistNode

	if (!iter->current) {
		D("Returning because current node is NULL")
			return 0;
	}

	//函数指针，返回值和两个参数都是unsigned char *类型
	unsigned char *(*nextFn)(unsigned char *, unsigned char *) = NULL;
	int offset_update = 0;

	//如果迭代器的zi指针指向的ziplist为空，则要根据offset设置
	if (!iter->zi) {
		/* If !zi, use current index. */
		quicklistDecompressNodeForUse(iter->current);   //临时解压缩
		iter->zi = ziplistIndex(iter->current->zl, iter->offset);   //根据ziplist中偏移量，这只zi指向
	} else {
		/* else, use existing iterator offset and get prev/next as necessary. */
		if (iter->direction == AL_START_HEAD) { //如果是正向迭代，则设置指向next的函数指针
			nextFn = ziplistNext;
			offset_update = 1;                          //向后走1个，记录该值
		} else if (iter->direction == AL_START_TAIL) {//如果是反向迭代，则设置指向prev的函数指针
			nextFn = ziplistPrev;
			offset_update = -1;                         //向前走1个，记录该值
		}
		iter->zi = nextFn(iter->current->zl, iter->zi); //更新zi指向的ziplist中的节点
		iter->offset += offset_update;                  //更新偏移量
	}

	//更新quicklistEntry结构中的信息
	entry->zi = iter->zi;
	entry->offset = iter->offset;

	//如果迭代到的下一个entry节点不为空
	if (iter->zi) {
		/* Populate value from existing ziplist position */
		//将当前entry节点的信息读到quicklistEntry中
		ziplistGet(entry->zi, &entry->value, &entry->sz, &entry->longval);
		return 1;
	} else {    //如果已经迭代完了ziplist
		/* We ran out of ziplist entries.
		 * Pick next node, update offset, then re-run retrieval. */
		quicklistCompress(iter->quicklist, iter->current);  //按需压缩
		if (iter->direction == AL_START_HEAD) { //如果正向迭代，将迭代器指向下一个节点
			/* Forward traversal */
			D("Jumping to start of next node");
			iter->current = iter->current->next;
			iter->offset = 0;
		} else if (iter->direction == AL_START_TAIL) {//如果反向迭代，将迭代器指向上一个节点
			/* Reverse traversal */
			D("Jumping to end of previous node");
			iter->current = iter->current->prev;
			iter->offset = -1;
		}
		iter->zi = NULL;
		return quicklistNext(iter, entry);
	}
}

