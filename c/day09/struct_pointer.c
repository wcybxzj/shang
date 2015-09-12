#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define NUM 4
#define BUFFSIZE 100

typedef struct stu_st{
	int id;
	char name[32];
	int ch;
	int eng;
}stu_t;

void func(const void *p)
{
	const stu_t *tmp = p;
	printf("%d\n", tmp->id);
}

int main(int argc, const char *argv[])
{
	int i;
	char str[BUFFSIZE] = {};
	srand(time(NULL));
	stu_t *p = malloc(sizeof(stu_t) * NUM);
	for (i = 0; i < NUM; i++) {
		p[i].id = rand()%1000001;
		sprintf(str,"ybx-%d",rand()%1000001);
		strcpy(p[i].name, str);
		p[i].ch = rand()%101;
		p[i].eng = rand()%101;
	}

	//stu_t p[] = {
	//	{1, "zhanghuan", 90, 99},
	//	{5, "sujunjie", 98, 77},
	//	{7, "liuyufeng", 99, 66},
	//	{10, "zhangning", 98, 89}
	//};


	printf("=========2的数据=========\n");

	printf("-----------p的地址-------------\n");
	printf("%p\n",p);//0x7fee58404c80

	printf("-----p+2*sizeof(stu_t)------\n");
	printf("%p\n", p+2*sizeof(stu_t));//0x7fee58405ba0

	//重要的结论 如果结构体数组提要按照字节去操作
	//需要先换成 无类型的指针
	const void *p1 = p;
	const stu_t *d = p1+2*sizeof(stu_t);

	printf("-----p1+2*sizeof(stu_t)------\n");
	printf("%p\n", p1+2*sizeof(stu_t));//0x7fee58404cd8

	printf("%d\n", d->id);



	//printf("=========循环输出=========\n");
	//for (i = 0; i < NUM; i++) {
	//	printf("id: %d\n", p[i].id);
	//	printf("name: %s\n", p[i].name);
	//	printf("ch: %d\n", p[i].ch);
	//	printf("eng: %d\n", p[i].eng);
	//}

	//printf("=======1的数据===========\n");
	//p+=1;
	//printf("id: %d\n", (*p).id);
	//printf("id: %d\n", p->id);
	//printf("name: %s\n", (*p).name);
	//printf("ch: %d\n", (*p).ch);
	//printf("eng: %d\n", (*p).eng);


	return 0;
}
