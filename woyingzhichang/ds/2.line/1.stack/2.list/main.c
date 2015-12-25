#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

#define NAMESIZE 32
typedef struct _SCORE {
	int id;
	char name[NAMESIZE];
	int math;
	int chinese;
} SCORE_ST;

void myprint(const void *data)
{
	const SCORE_ST * cur = data;
	printf("%d %s %d %d\n",\
			cur->id, cur->name, cur->math, cur->chinese);
}

int cmp_id(const void *key, const void *data){
	const int* id = key;
	const SCORE_ST * p= data;
	return *id - (p->id);
}

int cmp_name(const void *key, const void *data){
	const char *name = key;
	const SCORE_ST * p= data;
	return strcmp(name, p->name);
}

//变长结构体版本
//类的概念:数据和方法都放入头节点
int main(int argc, const char *argv[])
{
	int i, ret;
	STACK *me;
	SCORE_ST tmp;
	me = stack_create(sizeof(SCORE_ST));
	if (NULL == me) {
		exit(1);
	}

	for (i = 0; i < 7; i++) {
		tmp.id = i;
		snprintf(tmp.name, NAMESIZE, "stu%d", i);
		tmp.math = rand() % 100;
		tmp.chinese = rand() % 100;
		if (stack_push(me, &tmp) == 0){
			printf("push ok\n");
		}else{
			printf("push failed\n");
			break;
		}
	}

	while (1) {
		if (stack_pop(me, &tmp)){
			myprint(&tmp);
		}else{
			printf("pop failed\n");
			break;
		}
	}

	stack_destroy(me);
	exit(0);
}
