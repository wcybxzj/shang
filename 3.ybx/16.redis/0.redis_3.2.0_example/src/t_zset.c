#include "server.h"
#include <math.h>

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

int zslRandomLevel(void) {
    int level = 1;
    while ((random()&0xFFFF) < (ZSKIPLIST_P * 0xFFFF))
        level += 1;
    return (level<ZSKIPLIST_MAXLEVEL) ? level : ZSKIPLIST_MAXLEVEL;
}

zskiplistNode *zslInsert(zskiplist *zsl, double score, robj *obj) {
	// updata[]数组记录每一层位于插入节点的前一个节点
	zskiplistNode *update[ZSKIPLIST_MAXLEVEL], *x;
	// rank[]记录每一层位于插入节点的前一个节点的排名
	unsigned int rank[ZSKIPLIST_MAXLEVEL];
	int i, level;
	//serverAssert(!isnan(score));
	assert(!isnan(score));
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
	level = zslRandomLevel();
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
    //serverAssert(x != NULL);
	assert(x!=NULL);

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

