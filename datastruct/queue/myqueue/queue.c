#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

QUEUE *init_queue(int size){
	return init_llist(size);
}

status_t in_queue(QUEUE *myqueue, const void *data){
	return insert_llist(myqueue, data,REAR);	
}


static int always(const void *d1, const void *d2)
{
	return 0;
}


status_t out_queue(QUEUE *myqueue, void *data){
	return fetch_llist(myqueue, NULL, data, always);
}

status_t empty_queue(QUEUE *myqueue){
	return empty_llist(myqueue);
}

void destroy_queue(QUEUE *myqueue){
	destroy_llist(myqueue);
}
