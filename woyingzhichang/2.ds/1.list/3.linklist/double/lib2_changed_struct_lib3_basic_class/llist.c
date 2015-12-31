#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "llist.h"

HEAD *llist_creat(int size){
	HEAD *me;
	me = malloc(sizeof(*me));
	if (NULL ==me) {
		return NULL;
	}
	me->size = size;
	me->head.prev = &me->head;
	me->head.next = &me->head;

	//没意义因为create的时候还没有结构体
	//这里的绑定没意义
	//me->create = llist_creat;
	me->insert = llist_insert;
	me->find = llist_find;
	me->delete = llist_delete;
	me->fetch = llist_fetch;
	me->travel = llist_travel;
	me->destroy = llist_destroy;
	return me;
}

int llist_insert(HEAD *ptr, const void *data, int mode){
	NODE_ST *q;
	q = malloc(sizeof(*q)+ptr->size);
	if (NULL == q) {
		return -1;
	}
	memcpy(q->data, data, ptr->size);

	NODE_ST *p = &ptr->head;
	if (mode == LLIST_FORWARD) {
		q->prev = p;
		q->next = p->next;
	}else{
		q->next = p;
		q->prev = p->prev;
	}
	q->prev->next = q;
	q->next->prev = q;
	return 0;
}

NODE_ST *find_(HEAD *ptr, const void *key, cmp_t *func){
	NODE_ST *cur = NULL;
	for (cur = ptr->head.prev; cur != &ptr->head; cur = cur->prev) {
		if (func(key, cur->data)==0) {
			return cur;
		}
	}
	return NULL;
}

void *llist_find(HEAD *ptr, const void *key, cmp_t *func){
	NODE_ST *cur = find_(ptr, key, func);
	if (cur) {
		return cur->data;
	} else {
		return NULL;
	}
}

int llist_delete(HEAD *ptr, const void *key, cmp_t *func){
	NODE_ST *cur = NULL;
	cur = find_(ptr, key, func);
	if (cur) {
		cur->prev->next = cur->next;
		cur->next->prev = cur->prev;
		free(cur);
		return 1;
	}
	return 0;
}

int llist_fetch(HEAD *ptr, const void *key, cmp_t *func, void *re){
	NODE_ST *cur;
	cur = find_(ptr, key, func);
	if (cur) {
		memcpy(re, cur->data, ptr->size);
		cur->prev->next = cur->next;
		cur->next->prev = cur->prev;
		free(cur);
		return 1;
	}
	return 0;
}

void llist_travel(HEAD *ptr, print_t pr){
	NODE_ST *cur;
	for (cur = ptr->head.prev; cur != &ptr->head; cur = cur->prev) {
		pr(cur->data);
	}
}

void llist_destroy(HEAD* ptr){
	NODE_ST *cur, *next;
	for (cur = ptr->head.next; cur != &(ptr->head); cur = next ) {
		next = cur->next;
		free(cur);
	}
	free(ptr);
}
