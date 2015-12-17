#include <stdio.h>
#include <stdlib.h>
#include "nohead.h"

//只实现首部插入
struct node_st *list_insert(struct node_st *list, STU *data){
	struct node_st *new;
	new = malloc(sizeof(*new));
	if (NULL == new) {
		return NULL;
	}
	new->data = *data;
	new->next = list;
	return new;
}

void list_show(NODE *list){
	NODE *cur;
	for (cur = list; cur != NULL; cur = cur->next) {
		printf("id:%d ", cur->data.id);
		printf("name:%s ", cur->data.name);
		printf("math:%d ", cur->data.math);
		printf("math:%d \n", cur->data.math);
	}
}

//list_delete();
//list_find();
