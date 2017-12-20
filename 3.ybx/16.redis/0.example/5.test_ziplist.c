#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "zmalloc.h"
#include "sds.h"
#include "adlist.h"
#include "dict.h"
#include "ziplist.h"

unsigned char *createList() {
	unsigned char *zl = ziplistNew();
	zl = ziplistPush(zl, (unsigned char*)"foo", 3, ZIPLIST_TAIL);
	zl = ziplistPush(zl, (unsigned char*)"quux", 4, ZIPLIST_TAIL);
	zl = ziplistPush(zl, (unsigned char*)"hello", 5, ZIPLIST_HEAD);
	zl = ziplistPush(zl, (unsigned char*)"1024", 4, ZIPLIST_TAIL);
	return zl;
}

int main(int argc, const char *argv[])
{
	unsigned char*zl = createList();
	ziplistRepr(zl);
	return 0;
}

