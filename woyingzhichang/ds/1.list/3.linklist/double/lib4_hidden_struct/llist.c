#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "llist.h"

typedef struct _NODE {
	struct _NODE* prev;
	struct _NODE* next;
	char data[1];
} NODE_ST;

typedef struct _LLIST{
	int size;
	NODE_ST head;
}INNER_LLIST;

LLIST *llist_creat(int size){
	INNER_LLIST *me;
	me = malloc(sizeof(*me));
	if (NULL ==me) {
		return NULL;
	}
	me->size = size;
	me->head.prev = &me->head;
	me->head.next = &me->head;
	return me;
}

int llist_insert(LLIST *ptr1, const void *data, int mode){
	INNER_LLIST *ptr = ptr1;
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

NODE_ST *find_(LLIST *ptr1, const void *key, cmp_t *func){
	INNER_LLIST *ptr = ptr1;
	NODE_ST *cur = NULL;
	for (cur = ptr->head.prev; cur != &ptr->head; cur = cur->prev) {
		if (func(key, cur->data)==0) {
			return cur;
		}
	}
	return NULL;
}

void *llist_find(LLIST *ptr1, const void *key, cmp_t *func){
	INNER_LLIST *ptr = ptr1;
	NODE_ST *cur = find_(ptr, key, func);
	if (cur) {
		return cur->data;
	} else {
		return NULL;
	}
}

int llist_delete(LLIST *ptr1, const void *key, cmp_t *func){
	INNER_LLIST *ptr = ptr1;
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

int llist_fetch(LLIST *ptr1, const void *key, cmp_t *func, void *re){
	INNER_LLIST *ptr = ptr1;
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

void llist_travel(LLIST *ptr1, print_t pr){
	INNER_LLIST *ptr = ptr1;
	NODE_ST *cur;
	for (cur = ptr->head.prev; cur != &ptr->head; cur = cur->prev) {
		pr(cur->data);
	}
}

void llist_destroy(LLIST* ptr1){
	INNER_LLIST *ptr = ptr1;
	NODE_ST *cur, *next;
	for (cur = ptr->head.next; cur != &(ptr->head); cur = next ) {
		next = cur->next;
		free(cur);
	}
	free(ptr);
}
