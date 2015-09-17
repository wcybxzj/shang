#include <stdlib.h>
#include <string.h>

#include "llist.h"

LLIST *init_llist(int size)
{
	LLIST *me = NULL;

	me = calloc(1, sizeof(*me));	
	if (NULL == me) {
		return NULL;
	}
	me->size = size;
	me->head.data = NULL;
	me->head.prev = me->head.next = &me->head;

	return me;
}

status_t insert_llist(LLIST *ptr, const void *data, way_t way)
{
	struct node_st *new = NULL;

	new = calloc(1, sizeof(*new));
	if (NULL == new)
		return FAIL;
	new->data = calloc(1, ptr->size);
	if (NULL == new->data) {
		free(new);
		return FAIL;
	}
	memcpy(new->data, data, ptr->size);

	if (way == FRONT) {
		new->prev = &ptr->head;
		new->next = ptr->head.next;
	} else {
		new->prev = ptr->head.prev;
		new->next = &ptr->head;
	}
	new->prev->next = new;
	new->next->prev = new;

	return OK;
}

status_t delete_llist(LLIST *ptr, const void *key, compare_t cmp)
{
	struct node_st *cur = NULL;	

	for (cur = ptr->head.prev; cur != &ptr->head; cur = cur->prev) {
		if (!cmp(key, cur->data))
			break;
	}
	if (cur == &ptr->head) 
		return FAIL;
	cur->prev->next = cur->next;
	cur->next->prev = cur->prev;
	free(cur->data);
	free(cur);

	return OK;
}

void *search_llist(LLIST *ptr, const void *key, compare_t cmp)
{
	struct node_st *cur = NULL;	
	for (cur = ptr->head.prev; cur != &ptr->head; cur = cur->prev) {
		if (!cmp(key, cur->data)) 
			return cur->data;
	}

	return NULL;
}

status_t fetch_llist(LLIST *ptr, const void *key, void *data, compare_t cmp) 
{
	struct node_st *tmp = NULL;

	for (tmp = ptr->head.next; tmp != &ptr->head; tmp = tmp->next) {
		if (!cmp(key, tmp->data)) {
			memcpy(data, tmp->data, ptr->size);
			tmp->prev->next = tmp->next;
			tmp->next->prev = tmp->prev;
			free(tmp->data);
			free(tmp);
			return OK;
		}
	}

	return FAIL;
}

status_t get_firstnode(LLIST *ptr, void *data)
{
	if (ptr->head.next == &ptr->head)
		return FAIL;
	memcpy(data, (ptr->head.next)->data, ptr->size);
	return OK;
}

void traval_llist(LLIST *ptr, traval_t op)
{
	struct node_st *cur = NULL;

	for (cur = ptr->head.next; cur != &ptr->head; cur = cur->next) {
		op(cur->data);
	}
}

int get_listnum(LLIST *ptr)
{
	int ret = 0;
	struct node_st *cur = NULL;

	for (cur = ptr->head.next; cur != &ptr->head; cur = cur->next) {
		ret ++;	
	}		

	return ret;
}

static int alway_cmp(const void *d1, const void *d2)
{
	return 0;
}

void reverse_llist(LLIST **ptr)
{
	LLIST *new;
	char *data;

	new = init_llist((*ptr)->size);
	data = malloc((*ptr)->size);
	if (NULL == data)
		return;
	
	while ((*ptr)->head.next != &(*ptr)->head) {
		fetch_llist(*ptr, NULL, data, alway_cmp);
		insert_llist(new, data, FRONT);
	}
	free(*ptr);
	*ptr = new;
}

void reverse_r(LLIST *ptr)
{
	char *data;
	data = malloc(ptr->size);
	//if error
	if (ptr->head.next == &ptr->head)
		return;
	fetch_llist(ptr, NULL, data, alway_cmp);
	reverse_r(ptr);
	insert_llist(ptr, data, REAR);
	free(data);
}

status_t empty_llist(LLIST *ptr)
{
	return ptr->head.next == &ptr->head && ptr->head.prev == &ptr->head;	
}

void destroy_llist(LLIST *ptr)
{
	struct node_st *cur = NULL;
	struct node_st *after = NULL;

	for (cur = ptr->head.next; cur != &ptr->head; cur = after) {
			after = cur->next;
			free(cur->data);
			free(cur);
	}

	free(ptr);
}

