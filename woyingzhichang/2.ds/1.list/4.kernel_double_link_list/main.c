#include <stdio.h>
#include <stdlib.h>
#include "list.h"

#define NAMESIZE 1024

typedef struct score_st {
	int id;
	char name[NAMESIZE];
	struct list_head node;
	int math;
	int chinese;
}SC;

int main(int argc, const char *argv[])
{
	int i;
	SC *datap;
	struct list_head *pos = NULL;
	LIST_HEAD(head);
	for (i = 0; i < 7; i++) {
		datap = malloc(sizeof(*datap));
		if (NULL == datap) {
			exit(1);
		}
		datap->id = i;
		snprintf(datap->name, NAMESIZE, "stu%d", i);
		datap->math = rand() %100;
		datap->chinese = rand() %100;
		list_add(&datap->node, &head);
	}

	list_for_each_entry(datap, &head, node) {
		printf("%d %s %d %d\n",
				datap->id, datap->name, datap->math, datap->chinese);
	}

	list_for_each(pos, &head){
		datap = list_entry(pos, SC, node);
		printf("%d %s %d %d\n",
				datap->id, datap->name, datap->math, datap->chinese);
	}

	exit(0);
}
