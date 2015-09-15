#include <stdio.h>
#include <stdlib.h>

#include "list.h"

static void print_f(const void *data)
{
	const int *d = data;

	printf("%d ", *d);
}

static int data_cmp(const void *key, const void *data)
{
	const int *k = key;
	const int *d = data;

	return *k - *d;
}

int main(void)
{
	LIST *jose = NULL;
	struct node_st *cur, *after;
	int i;

	jose = init_head(sizeof(int));	
	if (jose == NULL)
		exit(1);

	for (i = 1; i <= 10; i++) {
		insert_list(jose, &i, REAR);
	}
	traval_list(jose, print_f);
	printf("\n");

	cur = jose->head.next;
	while (1) {
		if ((cur->next == &jose->head) && (jose->head.next == cur))
			break;
		for (i = 0; i < 2; i++) {
			cur = cur->next;
			if (cur == &jose->head)
				cur = cur->next;
			after = cur->next;
			if (after == &jose->head)
				after = after->next;
		}
		delete_list(jose, cur->data, data_cmp);
		cur = after;
	}
	printf("%d was alive\n", *((int*)(cur->data)));

	destroy_list(jose);

	return 0;
}
