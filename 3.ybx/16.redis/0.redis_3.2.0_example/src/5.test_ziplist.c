#include "server.h"

#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <locale.h>
#include <sys/socket.h>

unsigned char *test_createList() {
	unsigned char *zl = ziplistNew();
	unsigned char buf[32];
	sprintf(buf, "100");
	zl = ziplistPush(zl, (unsigned char*)buf, strlen(buf), ZIPLIST_TAIL);
	sprintf(buf, "128000");
	zl = ziplistPush(zl, (unsigned char*)buf, strlen(buf), ZIPLIST_TAIL);
	char v1[257]={'\0'};
	memset(v1,'x',256);
	zl = ziplistPush(zl,(unsigned char*)v1,strlen(v1),ZIPLIST_HEAD);
	return zl;
}

//测试插入数据后引起后续节点连续重新分配空间的情况
unsigned char *test_ziplistCascadeUpdate(unsigned char *zl)
{
	//创造几个节点整体长度是253的节点,1+2+250
	//然后插入在最前面插入一个
	char v1[251]={'\0'};
	memset(v1,'x',250);
	zl = ziplistPush(zl,(unsigned char*)v1,strlen(v1),ZIPLIST_TAIL);
	memset(v1,'y',250);
	zl = ziplistPush(zl,(unsigned char*)v1,strlen(v1),ZIPLIST_TAIL);
	memset(v1,'z',250);
	zl = ziplistPush(zl,(unsigned char*)v1,strlen(v1),ZIPLIST_TAIL);
	//然后在头部插入一个总长度为254的节点,1+2+251=254,
	//所以后边的节点需要扩容previous_entry_length从1字节到5字节来保存前面的节点的长度
	char v2[252]={'\0'};
	memset(v2,'c',251);
	zl = ziplistPush(zl,(unsigned char*)v2,strlen(v2),ZIPLIST_HEAD);
	return zl;
}

void *test_search(unsigned char *zl){
	zl = ziplistIndex(zl, ZIPLIST_HEAD);
	get(zl);
}

void get(unsigned char *zl)
{
	unsigned char *str;
	char *result_str[1000]={};
	int str_len;
	int num;
	if (ziplistGet(zl, &str, &str_len, &num)) {
		if (str!=NULL) {
			strncpy(result_str, str, str_len);
			printf("%s\n",result_str);
		}else{
			printf("%d\n",num);
		}
	}
}

unsigned char *test_delete(unsigned char *zl)
{
	unsigned char *find_zl=NULL;
	find_zl = ziplistIndex(zl, ZIPLIST_HEAD);
	get(find_zl);
	if (find_zl==NULL) {
		printf("can not get this ziplist node\n");
		exit(1);
	}

	ziplistDelete(zl, &find_zl);
	get(find_zl);
}

int main(int argc, const char *argv[])
{
	unsigned char *zl;

	//zl = test_createList(zl);
	//ziplistRepr(zl);

	zl = ziplistNew();
	zl = test_ziplistCascadeUpdate(zl);
	//ziplistRepr(zl);

	//test_search(zl);
	zl = test_delete(zl);
	//ziplistRepr(zl);

	return 0;
}

