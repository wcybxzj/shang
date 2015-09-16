#include <stdlib.h>
#include <string.h>
#include <llist.h>

LLIST *init_llist(int size){
	LLIST *you = NULL;
	you = malloc(sizeof(LLIST));
	if (NULL == you) {
		return NULL;
	}
	memset(you, 0x00, sizeof(*you));
	you->head.data = NULL; 
	you->head.prev = &you->head;
	you->head.next = &you->head;
	you->size = size;
	return you;
}

status_t insert_llist(LLIST *ptr, const void *data, way_t way){
	struct node_st *new = NULL;
	struct node_st *rear = NULL;

	//new
	new = malloc(sizeof(*you));
	if (NULL == new) {
		return FAIL;
	}
	memset(ptr, 0x00, sizeof(*you));

	//data
	new->data = malloc(ptr->size);
	if (NULL == new->data) {
		free(new);
		return FAIL;
	}

	//memcpy
	memcpy(new->data, data, ptr->size);

	//font/rear
	if (way == FRONT) {
		if (len==0) {
			new->next = ptr->head.next;
			new->prev = &ptr->head;
			ptr->head.next = new;
			ptr->head.prev = new;
		}else{

		}
	}
}

void traval_llist(LLIST *ptr, traval_t op){

}
