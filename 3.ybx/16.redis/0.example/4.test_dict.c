#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "zmalloc.h"
#include "sds.h"
#include "adlist.h"
#include "dict.h"

static unsigned int callbackHash(const void *key) {
    return dictGenHashFunction((const unsigned char *)key,
                               strlen((const char *)key));
}

static int callbackKeyCompare(void *privdata, const void *key1, const void *key2) {
    int l1, l2;
    ((void) privdata);

    l1 = strlen((const char *)key1);
    l2 = strlen((const char *)key2);
    if (l1 != l2) return 0;
    return memcmp(key1,key2,l1) == 0;
}

static dictType callbackDict = {
    callbackHash,
    NULL,
    NULL,
    callbackKeyCompare,
    NULL,
	NULL
};

void test_dict()
{
	dict *dict_p = dictCreate(&callbackDict,NULL);
	dictAdd(dict_p, "key:111","val:111");
	dictAdd(dict_p, "key:222","val:222");

	char *v1 = dictFetchValue(dict_p,"key:111");
	char *v2 = dictFetchValue(dict_p,"key:222");

	printf("%s\n",v1);
	printf("%s\n",v2);
}

int main(int argc, const char *argv[])
{


	test_dict();
	return 0;
}

