#include <stdio.h>
#include <stdlib.h>
#include "list.h"

list *list_create(){
	list *me;
	me = malloc(sizeof(*me));
	if (NULL == me) {
		return NULL;
	}
	me->next = NULL;
	return me;
}

list *get_prev_list(list *ptr, int i){
	int tmp = 1;
	while (ptr->next) {
		if (tmp == i) {
			return ptr;
		}
		ptr = ptr->next;
		tmp++;
	}
	return NULL;
}

int list_insert(list *ptr, int i, datatype *data){
	list *prev_node, *node, *tmp_node;
	prev_node = get_prev_list(ptr, i);
	if (NULL == prev_node) {
		return -1;
	}

	node = malloc(sizeof(*node));
	if (NULL == node) {
		return -2;
	}
	node->data = *data;
	node->next = NULL;

	tmp_node = prev_node->next;
	node->next = tmp_node;
	prev_node->next = node;

	return 1;
}

//int list_order_insert(list *, datatype *){
//}

void list_display(list *ptr){
	while (ptr->next) {
		printf("%d ", ptr->data);
	}
	printf("\n");
}

//int list_delete_at(list *ptr, int i, datatype *data){
//}
//
//int list_delete(list *, datatype *){
//}
//int list_isempty(list *){
//}
//
//void list_destory(list *){
//}
