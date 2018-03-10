#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "zmalloc.h"
#include "sds.h"
#include "adlist.h"
#include "dict.h"

void test_sds();
void test_sdssplitlen();

int main(int argc, const char *argv[])
{
	test_sds();
	//test_sdssplitlen();
	return 0;
}

void test_sds(){
	char *str= "abcdefhijklmnopqrstuvwxyz1234567890abcdefghij";
	printf("%d\n",strlen(str));//45
	printf("当前reids内存用量:%d\n", zmalloc_used_memory());//0
	sds sds_obj = sdsnew(str);
	printf("当前reids内存用量:%d\n", zmalloc_used_memory());//64
	char *str2= "haha";
	sds_obj = sdscatlen(sds_obj, str2, strlen(str2));
	printf("%s\n",sds_obj);

	sdsrange(sds_obj,0,10);
	printf("%s\n",sds_obj);

}

/*
[root@web11 0.example]# ./main
测试1:单个分隔符
return_count:3
空字符串
b
c
=============================================
测试2:多个分隔符
return_count:4
空字符串
123
45a6
空字符串
*/
void test_sdssplitlen()
{
	int i;
	int return_count;
	sds *lines;
	char *str= "abac";
	char *sep="a";

	sds sds_obj = sdsnew(str);
	lines = sdssplitlen(str,strlen(str),sep,strlen(sep),&return_count);

	printf("return_count:%d\n",return_count);
	for (i = 0; i < return_count; i++) {
		printf("%s\n",lines[i]);
	}

	printf("=============================================\n");

	str="ab123ab45a6ab";
	sep="ab";
	sds_obj = sdsnew(str);
	lines = sdssplitlen(str,strlen(str),sep,strlen(sep),&return_count);

	printf("return_count:%d\n",return_count);
	for (i = 0; i < return_count; i++) {
		printf("%s\n",lines[i]);
	}

}

