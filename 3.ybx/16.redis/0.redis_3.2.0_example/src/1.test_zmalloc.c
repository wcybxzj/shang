#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "zmalloc.h"
#include "sds.h"
#include "adlist.h"
#include "dict.h"

void test_zmalloc();

int main(int argc, const char *argv[])
{
#ifndef HAVE_MALLOC_SIZE
	printf("在我现在这里是没有定义这个变量 HAVE_MALLOC_SIZE\n");
#endif
	test_zmalloc();
	return 0;
}

void test_zmalloc(){
	void *str = NULL;
	str = zmalloc(10);
	if (str==NULL) {
		perror("zmalloc()");
		exit(1);
	}
	strcpy(str,"abc");
	printf("%s\n",str);
	printf("str实际占用内存空间:%d\n", zmalloc_size(str));//24 (笔记zee-redis内存分配)
}


