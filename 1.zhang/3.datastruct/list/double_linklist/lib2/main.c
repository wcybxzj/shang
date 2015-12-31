#include <stdio.h>
#include <stdlib.h>

#include "llist.h"

static void fun_print(const void *data)
{
	const int *d = data;

	printf("%d ", *d);
}

static int id_cmp(const void *key, const void *data)
{
	const int *m = key;
	const int *n = data;

	return *m - *n;
}

int main(void)
{
	LLIST *my = NULL;
	int id[] = {3,2,6,9,8,1,7,4,5};
	int i;
	int del;
	int *p;

	my = init_llist(sizeof(int));
	if (NULL == my) {
		exit(1);
	}
	
	for (i = 0; i < sizeof(id)/sizeof(*id); i++) {
		insert_llist(my, id+i, REAR);
	}
	traval_llist(my, fun_print);
	printf("\n\n");

#if 0
	del = 1;
	delete_llist(my, &del, id_cmp);
	traval_llist(my, fun_print);
	printf("\n\n");
	del = 5;
	p = search_llist(my, &del, id_cmp);
	if (NULL == p) {
		printf("not found\n");
	} else {
		printf("find it: %d\n", *p);
	}

	printf("***********fetch************\n");
	i = 4;
	if ( fetch_llist(my, &i, &del, id_cmp) < 0) {
		printf("fetch failed\n");
	}
	printf("fetch:%d\n", del);
	traval_llist(my, fun_print);
	printf("\n\n");

#endif

	printf("the number of linklist is %d\n", get_listnum(my));
	reverse_llist(&my);
	traval_llist(my, fun_print);
	printf("\n\n");

	reverse_r(my);
	traval_llist(my, fun_print);
	printf("\n\n");

	destroy_llist(my);
	return 0;
}
