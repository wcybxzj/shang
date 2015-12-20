#include <stdio.h>
#include <stdlib.h>
#include "nohead.h"
#include <string.h>

//只实现首部插入
int list_insert(NODE **list, STU *data){
	struct node_st *new;
	new = malloc(sizeof(*new));
	if (NULL == new) {
		return -1;
	}
	new->data = *data;
	new->next = *list;
	*list = new;
	return 0;
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

//只实现首部删除
int list_delete(NODE **list){
	NODE *tmp = NULL;
	if (*list == NULL) {
		return -1;
	}
	if((*list)->next) {
		tmp = *list;
		*list = (*list)->next;
		free(tmp);
	}
	//free(*list);
	//*list = NULL;
	return 0;
}

int list_find(NODE *ptr, int id, STU *ret_ptr ){
	NODE *tmp = ptr;
	for ( ; tmp; tmp = tmp->next) {
		if (tmp->data.id == id) {
			memcpy(ret_ptr, tmp, sizeof(STU));
			return 0;
		}
	}
	return -1;
}

//只实现首部删除
int list_destroy(NODE **list){
	NODE *tmp = NULL, *cur = *list;
	for (; cur; cur = tmp ) {
		tmp = cur->next;
		free(cur);
	}
	*list = NULL;
	return 0;
}
