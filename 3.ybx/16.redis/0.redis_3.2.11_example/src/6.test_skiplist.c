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
#define MAXNUM 5

int zslValueGteMin(double value, zrangespec *spec);


typedef struct _USER {
	int age;
	char *name;
} USER;


//只能处理object里sds是sdshdr8类型的情况
char *get_string_from_StringObject(robj*o)
{
	struct sdshdr8 *sh = (void*)(o+1);
	return sh->buf;
}

void insert(zskiplist * zsl)
{
	//插入
	int i;
	USER users[MAXNUM];
	users[0].name = "ly";
	users[0].age= 30;
	users[1].name = "wc";
	users[1].age= 33;
	users[2].name = "ybx";
	users[2].age= 31;
	users[3].name = "ypq";//可以换成yjm
	users[3].age= 88;
	users[4].name = "yjm";
	users[4].age= 14;

	for (i = 0; i < MAXNUM; i++) {
		robj * obj = createStringObject(users[i].name,strlen(users[i].name));
		zskiplistNode * p = zslInsert(zsl, users[i].age, obj);
		if (p==NULL) {
			printf("insert fail\n");
		}else{
			printf("insert ok\n");
		}
	}

	////再次插入上边5个对象说明可以插入完全相同的对象
	//for (i = 0; i < MAXNUM; i++) {
	//	robj * obj = createStringObject(users[i].name,strlen(users[i].name));
	//	zskiplistNode * p = zslInsert(zsl, users[i].age, obj);
	//	if (p==NULL) {
	//		printf("insert fail\n");
	//	}else{
	//		printf("insert ok\n");
	//	}
	//}


}

void foreach(zskiplist * zsl)
{
	//迭代所有元素
	//zrangeGenericCommand()
	int reverse = 0;
	int rangelen;
	int start =0;
	int end =-1;
	int llen= zsl->length;//5
	robj *ele;
	char *str;

	if (start < 0) start = llen+start;
	if (end < 0) end = llen+end;
	if (start < 0) start = 0;

	if (end >= llen) end = llen-1;
	rangelen = (end-start)+1;//5

	zskiplistNode *ln;
	ln = zsl->header->level[0].forward;
	//printf("start:%d, end:%d, rangelen:%d\n", start, end, rangelen);//0,4,5

	while(rangelen--) {
		ele = ln->obj;
		str = get_string_from_StringObject(ele);
		printf("%g\n", ln->score);
		printf("%s\n",str);
		ln = reverse ? ln->backward : ln->level[0].forward;
	}
}

/*
6.test_skiplist > /tmp/123
[root@web11 src]# sort /tmp/123 | uniq -dc
7498973 1
     30 10
      6 11
1876733 2
 468323 3
 116915 4
  29322 5
   7280 6
   1843 7
    443 8
    132 9
*/
//测试插入节点的分层情况
void test_zslRandomLevel()
{
	int i;
	int tmp;
	for (i = 0; i <10; i++) {
		//tmp = zslRandomLevel();
		tmp = ybx_zslRandomLevel();
		printf("%d\n",tmp);
	}
}

//主要参考:genericZrangebyscoreCommand() 和zslFirstInRange()
//测试:给出范围获取信息
void ZRANGEBYSCORE(zskiplist *zsl)
{
	zrangespec range;
	range.min = 30;
	range.max = 33;
	range.minex = 0;//值为 1 表示不包含，值为 0 表示包含
	range.maxex = 0;

	zskiplistNode *ln;
	ln = zslFirstInRange(zsl,&range);//获取第一个在范围内的元素
	if (ln==NULL) {
		printf("NULL\n");
		exit(1);
	}

	char* str;
	robj* ele;
	int reverse=0;//是否反向 0否
	while (ln ) {
		/* Abort when the node is no longer in range. */
		if (reverse) {
			if (!zslValueGteMin(ln->score,&range)) break;
		} else {
			if (!zslValueLteMax(ln->score,&range)) break;
		}

		//打印元素信息
		ele = ln->obj;
		str = get_string_from_StringObject(ele);
		printf("%g\n", ln->score);
		printf("%s\n",str);

		/* Move to next node */
		if (reverse) {
			ln = ln->backward;
		} else {
			ln = ln->level[0].forward;
		}
	}
}

void test_zslDelete(zskiplist *zsl)
{
	int ret;
	char *name = "ybx";
	int age= 31;
	robj * obj = createStringObject(name,strlen(name));
	ret = zslDelete(zsl, age, obj);
	if (ret) {
		printf("delete ok\n");
	}else{
		printf("delete fail\n");
		exit(1);
	}

	name = "ypq";
	age = 88;
	obj = createStringObject(name,strlen(name));
	ret = zslDelete(zsl, age, obj);
	if (ret) {
		printf("delete ok\n");
	}else{
		printf("delete fail\n");
		exit(1);
	}
}

//获取指定成员的排名
void test_zrank(zskiplist *zsl)
{
	int num;
	char *name = "ybx";
	int age= 31;
	robj * obj = createStringObject(name,strlen(name));
	num = zslGetRank(zsl ,age, obj);
	printf("num:%d\n",num);

	name = "wc";
	age= 33;
	obj = createStringObject(name,strlen(name));
	num = zslGetRank(zsl ,age, obj);
	printf("num:%d\n",num);


	name = "ypq";
	age= 88;
	obj = createStringObject(name,strlen(name));
	num = zslGetRank(zsl ,age, obj);
	printf("num:%d\n",num);
}

int main(int argc, const char *argv[])
{

	initServerConfig();
	createSharedObjects();
	//测试1:
	test_zslRandomLevel();
	exit(0);

	zskiplist * zsl= zslCreate();
	insert(zsl);

	//foreach(zsl);

	printf("===============================\n");
	//ZRANGEBYSCORE(zsl);
	printf("==============================\n");
	//foreach(zsl);
	//test_zslDelete(zsl);
	//foreach(zsl);

	test_zrank(zsl);
	return 0;
}

