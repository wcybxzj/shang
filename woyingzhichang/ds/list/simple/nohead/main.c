#include <stdio.h>
#include <stdlib.h>
#include "nohead.h"

int main(int argc, const char *argv[])
{
	NODE *list = NULL;
	STU tmp;
	int i, ret;
	for (i = 0; i < 7; i++) {
		tmp.id = i;
		snprintf(tmp.name, NAMESIZE, "stu%d", i);
		tmp.math = rand() % 100;
		tmp.chinese = rand() % 100;
		ret = list_insert(&list, &tmp);
		if (ret < 0) {
			fprintf(stderr, "error insert\n");
			exit(1);
		}
	}
	list_show(list);

	//
	ret = list_find(list, 2, &tmp);
	if (ret == 0) {
		printf("id:%d, math:%d, chiese:%d \n", tmp.id, tmp.math, tmp.chinese);
	}else {
		printf("not find id:%d\n", 2);
	}

	//
	list_delete(&list);
	list_show(list);

	//
	ret = list_destroy(&list);
	if (ret == 0) {
		printf("destroy ok\n");
	}
	list_show(list);

	return 0;
}
