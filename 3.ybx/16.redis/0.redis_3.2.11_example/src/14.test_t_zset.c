#include "14.test_t_zset.h"

void ybx_zaddGenericCommand(robj *zobj, double score, robj *elem_o)
{
	// 如果为ziplist
	if (zobj->encoding == OBJ_ENCODING_ZIPLIST) {
	
	} else if (zobj->encoding == OBJ_ENCODING_SKIPLIST) {
		zset *zs = zobj->ptr;
		zskiplistNode *znode;
		znode = zslInsert(zs->zsl,score,elem_o);
	}else{
		serverPanic("Unknown sorted set encoding");
	}
}

void ybx_zrangeGenericCommand(robj *zobj, long start, long end)
{
	int reverse = 0;
	int withscores = 1;
	int llen;
	int rangelen;

	llen = zsetLength(zobj);
	if (start < 0) start = llen+start;
	if (end < 0) end = llen+end;
	if (start < 0) start = 0;

	if (start > end || start >= llen) {
		return;
	}

	if (end >= llen) end = llen-1;
	rangelen = (end-start)+1;       //范围长度

	if (zobj->encoding == OBJ_ENCODING_ZIPLIST) {

	} else if (zobj->encoding == OBJ_ENCODING_SKIPLIST) {
		zset *zs = zobj->ptr;
		zskiplist *zsl = zs->zsl;
		zskiplistNode *ln;
		robj *ele;
		// 根据reverse决定，遍历的头或尾元素节点地址
		if (reverse) {
			ln = zsl->tail;
			if (start > 0)
				ln = zslGetElementByRank(zsl,llen-start);
		} else {
			ln = zsl->header->level[0].forward;
			if (start > 0)
				ln = zslGetElementByRank(zsl,start+1);
		}
		// 取出rangelen个元素
		while(rangelen--) {
			assert(ln!=NULL);
			ele = ln->obj;
			if (withscores)
				printf("%g\n",ln->score);
			// 指向下一个节点
			ln = reverse ? ln->backward : ln->level[0].forward;
		}
	}else{
		serverPanic("Unknown sorted set encoding");
	}
}


