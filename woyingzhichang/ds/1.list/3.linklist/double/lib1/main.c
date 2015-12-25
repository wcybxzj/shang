#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "llist.h"

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

int main(int argc, const char *argv[])
{
	int i, ret;
	LLIST *handler;
	SCORE_ST tmp, *tmp_p;
	handler = llist_creat(sizeof(SCORE_ST));

	for (i = 0; i < 7; i++) {
		tmp.id = i;
		snprintf(tmp.name, NAMESIZE, "stu%d", i);
		tmp.math = rand() %100;
		tmp.chinese = rand() %100;
		//myprint(&tmp);
		llist_insert(handler, &tmp, LLIST_FORWARD);
	}

	llist_travel(handler, myprint);

	printf("-----------------------------------\n");

	char *name = "stu3";
	tmp_p = llist_find(handler, name, cmp_name);
	myprint(tmp_p);

	printf("-----------------------------------\n");
	int id = 2;
	ret = llist_fetch(handler, &id, cmp_id, &tmp);
	if (ret) {
		printf("fetch ");
		myprint(&tmp);
	}

	printf("-----------------------------------\n");
	name ="stu0";
	ret = llist_delete(handler, name, cmp_name);
	if (ret) {
		printf("delete %s\n", name);
	}
	printf("-----------------------------------\n");
	llist_travel(handler, myprint);
	printf("-----------------------------------\n");

	name = malloc(NAMESIZE);
	for (i = 1; i < 7; i++) {
		snprintf(name, NAMESIZE, "stu%d", i);
		ret = llist_delete(handler, name, cmp_name);
		if (ret) {
			printf("delete %s\n", name);
		}
	}
	llist_travel(handler, myprint);

	llist_destroy(handler);
	return 0;
}
