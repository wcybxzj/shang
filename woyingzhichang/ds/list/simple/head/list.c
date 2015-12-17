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

int list_insert_at(list *me, int i, datatype *data){
	int j = 0;
	list *node = me, *newnode;
	if (i < 0){
		return -1;
	}

	//获取要插入的前驱
	//如果i比较大获取最接近的可用数值
	while (j < i && node->next) {
		j++;
		node = node->next;
	}

	if (node) {
		newnode = malloc(sizeof(*newnode));
		if (NULL == newnode) {
			return -3;
		}

		newnode->data = *data;
		newnode->next = node->next;
		node->next = newnode;
		return 0;
	}else{
		return -2;
	}
}

//按顺序插入
int list_order_insert(list *me, datatype *data){
	list *p = me, *q;
	while (p->next && (p->next->data < *data)) {
		p = p->next;
	}
	if (p) {
		q = malloc(sizeof(*q));
		if (NULL == q) {
			return -1;
		}
		q->data = *data;
		q->next = p->next;
		p->next = q;
		return 0;
	}else{
		return -2;
	}
}

void list_display(list *me){
	if (list_isempty(me) == TRUE) {
		return;
	}
	while (me->next) {
		me = me->next;
		printf("%d ", me->data);
	}
	printf("\n");
}

//*data 是删除元素的值
int list_delete_at(list *me, int i, datatype *data){
	list  *p = me, *q;
	int j = 0;

	*data = -1;

	if (i < 0) {
		return -1;
	}

	while (j < i && p->next) { 
		p = p->next;
		j++;
	}

	if (p) {
		q = p->next;
		p->next = q->next;
		*data = q->data;
		free(q);
		return 0;
	}else{
		return -2;
	}

}

int list_delete(list *me, datatype *data){
	list *p = me, *q;
	while (p->next && (p->next->data != *data)) {
		p = p->next;
	}
	if (p->next == NULL) {
		return -1;
	}else{
		q = p->next;
		p->next = p->next->next;
	}
	return 0;
}

int list_isempty(list *me){
	if (me->next) {
		return FALSE;
	}
	return TRUE;
}

void list_destory(list *me){
	list *ptr = me, *next_ptr;
	if (list_isempty(ptr) == TRUE) {
		return;
	}

	while (ptr->next) {
		next_ptr = ptr->next;
		free(ptr->next);
		ptr = next_ptr;

	}
	free(me);
}
