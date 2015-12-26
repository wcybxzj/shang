#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

Q *q_create(int initsize){
	return llist_create(initsize);
}

int q_enqueue(Q *ptr, void* data){
	return llist_insert(ptr, data, LLIST_BACKWARD);
}

int always(const void *p1, const void *p2){
	return 0;
}

int q_dequeue(Q *ptr, void* data){
	return llist_fetch(ptr, (void *)0, always, data);
}

void q_destroy(Q *ptr){
	llist_destroy(ptr);
}
