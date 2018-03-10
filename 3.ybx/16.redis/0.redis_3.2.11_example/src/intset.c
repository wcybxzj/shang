#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "intset.h"
#include "zmalloc.h"
#include "endianconv.h"


/* Return the required encoding for the provided value. */
uint8_t _intsetValueEncoding(int64_t v) {
	if (v < INT32_MIN || v > INT32_MAX)
		return INTSET_ENC_INT64;
	else if (v < INT16_MIN || v > INT16_MAX)
		return INTSET_ENC_INT32;
	else
		return INTSET_ENC_INT16;
}

/* Return the value at pos, given an encoding. */
static int64_t _intsetGetEncoded(intset *is, int pos, uint8_t enc) {
    int64_t v64;
    int32_t v32;
    int16_t v16;

    if (enc == INTSET_ENC_INT64) {
        memcpy(&v64,((int64_t*)is->contents)+pos,sizeof(v64));
        memrev64ifbe(&v64);
        return v64;
    } else if (enc == INTSET_ENC_INT32) {
        memcpy(&v32,((int32_t*)is->contents)+pos,sizeof(v32));
        memrev32ifbe(&v32);
        return v32;
    } else {
        memcpy(&v16,((int16_t*)is->contents)+pos,sizeof(v16));
        memrev16ifbe(&v16);
        return v16;
    }
}

/* Return the value at pos, using the configured encoding. */
static int64_t _intsetGet(intset *is, int pos) {
    return _intsetGetEncoded(is,pos,intrev32ifbe(is->encoding));
}

/* Set the value at pos, using the configured encoding. */
static void _intsetSet(intset *is, int pos, int64_t value) {
	uint32_t encoding = intrev32ifbe(is->encoding);
	if (encoding == INTSET_ENC_INT64) {
		((int64_t*)is->contents)[pos] = value;
		memrev64ifbe(((int64_t*)is->contents)+pos);
	} else if (encoding == INTSET_ENC_INT32) {
		((int32_t*)is->contents)[pos] = value;
		memrev32ifbe(((int32_t*)is->contents)+pos);
	} else {
		((int16_t*)is->contents)[pos] = value;
		memrev16ifbe(((int16_t*)is->contents)+pos);
	}
}

/* Create an empty intset. */
intset *intsetNew(void) {
	intset *is = zmalloc(sizeof(intset));
	is->encoding = intrev32ifbe(INTSET_ENC_INT16);
	is->length = 0;
	return is;
}

/* Resize the intset */
static intset *intsetResize(intset *is, uint32_t len) {
	uint32_t size = len*intrev32ifbe(is->encoding);
	is = zrealloc(is,sizeof(intset)+size);
	return is;
}

/* Search for the position of "value". Return 1 when the value was found and
 * sets "pos" to the position of the value within the intset. Return 0 when
 * the value is not present in the intset and sets "pos" to the position
 * where "value" can be inserted. */
static uint8_t intsetSearch(intset *is, int64_t value, uint32_t *pos) {
	int min = 0, max = intrev32ifbe(is->length)-1, mid = -1;
	int64_t cur = -1;

	/* The value can never be found when the set is empty */
	if (intrev32ifbe(is->length) == 0) {
		if (pos) *pos = 0;
		return 0;
	} else {
		/* Check for the case where we know we cannot find the value,
		 * but do know the insert position. */
		if (value > _intsetGet(is,intrev32ifbe(is->length)-1)) {
			if (pos) *pos = intrev32ifbe(is->length);
			return 0;
		} else if (value < _intsetGet(is,0)) {
			if (pos) *pos = 0;
			return 0;
		}
	}

	while(max >= min) {
		mid = ((unsigned int)min + (unsigned int)max) >> 1;
		cur = _intsetGet(is,mid);
		if (value > cur) {
			min = mid+1;
		} else if (value < cur) {
			max = mid-1;
		} else {
			break;
		}
	}

	if (value == cur) {
		if (pos) *pos = mid;
		return 1;
	} else {
		if (pos) *pos = min;
		return 0;
	}
}


/* Upgrades the intset to a larger encoding and inserts the given integer. */
static intset *intsetUpgradeAndAdd(intset *is, int64_t value) {
	uint8_t curenc = intrev32ifbe(is->encoding);
	uint8_t newenc = _intsetValueEncoding(value);
	int length = intrev32ifbe(is->length);
	int prepend = value < 0 ? 1 : 0;

	/* First set new encoding and resize */
	is->encoding = intrev32ifbe(newenc);
	is = intsetResize(is,intrev32ifbe(is->length)+1);

	/* Upgrade back-to-front so we don't overwrite values.
	 * Note that the "prepend" variable is used to make sure we have an empty
	 * space at either the beginning or the end of the intset. */
	while(length--)
		_intsetSet(is,length+prepend,_intsetGetEncoded(is,length,curenc));

	/* Set the value at the beginning or the end. */
	if (prepend)
		_intsetSet(is,0,value);
	else
		_intsetSet(is,intrev32ifbe(is->length),value);
	is->length = intrev32ifbe(intrev32ifbe(is->length)+1);
	return is;
}

static void intsetMoveTail(intset *is, uint32_t from, uint32_t to) {
	void *src, *dst;
	uint32_t bytes = intrev32ifbe(is->length)-from;
	uint32_t encoding = intrev32ifbe(is->encoding);

	if (encoding == INTSET_ENC_INT64) {
		src = (int64_t*)is->contents+from;
		dst = (int64_t*)is->contents+to;
		bytes *= sizeof(int64_t);
	} else if (encoding == INTSET_ENC_INT32) {
		src = (int32_t*)is->contents+from;
		dst = (int32_t*)is->contents+to;
		bytes *= sizeof(int32_t);
	} else {
		src = (int16_t*)is->contents+from;
		dst = (int16_t*)is->contents+to;
		bytes *= sizeof(int16_t);
	}
	memmove(dst,src,bytes);
}

/* Insert an integer in the intset */
intset *intsetAdd(intset *is, int64_t value, uint8_t *success) {
	uint8_t valenc = _intsetValueEncoding(value);
	uint32_t pos;
	if (success) *success = 1;

	/* Upgrade encoding if necessary. If we need to upgrade, we know that
	 * this value should be either appended (if > 0) or prepended (if < 0),
	 * because it lies outside the range of existing values. */
	if (valenc > intrev32ifbe(is->encoding)) {
		/* This always succeeds, so we don't need to curry *success. */
		return intsetUpgradeAndAdd(is,value);
	} else {
		/* Abort if the value is already present in the set.
		 * This call will populate "pos" with the right position to insert
		 * the value when it cannot be found. */
		if (intsetSearch(is,value,&pos)) {
			if (success) *success = 0;
			return is;
		}

		is = intsetResize(is,intrev32ifbe(is->length)+1);
		if (pos < intrev32ifbe(is->length)) intsetMoveTail(is,pos,pos+1);
	}

	_intsetSet(is,pos,value);
	is->length = intrev32ifbe(intrev32ifbe(is->length)+1);
	return is;
}


/* Determine whether a value belongs to this set
 *
 * 检查给定值 value 是否集合中的元素。
 *
 * 是返回 1 ，不是返回 0 。
 *
 * T = O(log N)
 */
uint8_t intsetFind(intset *is, int64_t value) {
    // 计算 value 的编码
    uint8_t valenc = _intsetValueEncoding(value);

    // 如果 value 的编码大于集合的当前编码，那么 value 一定不存在于集合
    // 当 value 的编码小于等于集合的当前编码时，
    // 才再使用 intsetSearch 进行查找
    return valenc <= intrev32ifbe(is->encoding) && intsetSearch(is,value,NULL);
}

/* Delete integer from intset */
intset *intsetRemove(intset *is, int64_t value, int *success) {
    uint8_t valenc = _intsetValueEncoding(value);
    uint32_t pos;
    if (success) *success = 0;

    if (valenc <= intrev32ifbe(is->encoding) && intsetSearch(is,value,&pos)) {
        uint32_t len = intrev32ifbe(is->length);

        /* We know we can delete */
        if (success) *success = 1;

        /* Overwrite value with tail and update length */
        if (pos < (len-1)) intsetMoveTail(is,pos+1,pos);
        is = intsetResize(is,len-1);
        is->length = intrev32ifbe(len-1);
    }
    return is;
}


/* Sets the value to the value at the given position. When this position is
 * out of range the function returns 0, when in range it returns 1. */
uint8_t intsetGet(intset *is, uint32_t pos, int64_t *value) {
    if (pos < intrev32ifbe(is->length)) {
        *value = _intsetGet(is,pos);
        return 1;
    }
    return 0;
}

/* Return intset length */
uint32_t intsetLen(intset *is) {    //返回集合的元素个数
    return intrev32ifbe(is->length);    //返回length成员
}

/* Return intset blob size in bytes. */
size_t intsetBlobLen(intset *is) {
    return sizeof(intset)+intrev32ifbe(is->length)*intrev32ifbe(is->encoding);
}

