#include <stdlib.h>
#include <string.h>
#include <list.h>

LIST *init_head(int size)
{
	LIST *me = NULL;

	me = malloc(sizeof(LIST));	
	if (NULL == me)
		return NULL;
	memset(me, 0x00, sizeof(LIST));
	me->size = size;
	me->head.next = &me->head;
	me->head.data = NULL;

	return me;
}

status_t insert_list(LIST *ptr, const void *data, way_t way)
{
	struct node_st *new = NULL;
	struct node_st *rear = NULL;

	new = malloc(sizeof(*new));
	if (NULL == new)
		return FAIL;
	memset(new, 0x00, sizeof(*new));
	new->data = malloc(ptr->size);
	if (new->data == NULL) {
		free(new);
		return FAIL;
	}
	memcpy(new->data, data, ptr->size);

	if (way == FRONT) {
		new->next = ptr->head.next;
		ptr->head.next = new;	
	} else {
		for (rear = ptr->head.next; rear->next!=&ptr->head; rear = rear->next);
		new->next = rear->next;
		rear->next = new;
	}		

	return OK;
}

status_t delete_list(LIST *ptr, const void *key, compare_t cmp)
{
	struct node_st *cur, *prev;

	for (cur = ptr->head.next, prev = &ptr->head; cur != &ptr->head; prev = cur, cur = cur->next) {
		if (!cmp(key, cur->data)) {
			prev->next = cur->next;
			cur->next = NULL;
			free(cur->data);
			free(cur);	
			return OK;
		}
	}		
	
	return FAIL;
}

void traval_list(LIST *ptr, print_t op)
{
	struct node_st *cur = NULL;

	for (cur = ptr->head.next; cur != &ptr->head; cur = cur->next){
		op(cur->data);	
	}
}

void destroy_list(LIST *ptr)
{
	struct node_st *cur, *after;		

	for (cur = ptr->head.next; cur != &ptr->head; cur = after) {
		after = cur->next;
		free(cur->data);
		free(cur);
	}
	free(ptr);
}

