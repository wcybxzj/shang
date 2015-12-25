#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

Q *q_create(void){
	Q *me = NULL;
	me = malloc(sizeof(*me));
	if (NULL == me) {
		return NULL;
	}
	me->head = me->tail = 0;
	return me;
}

int q_isempty(Q *ptr){
	return (ptr->head == ptr->tail);
}

int q_isfull(Q *ptr){
	return ((ptr->tail+1)%MAXSIZE == ptr->head);
}

int q_enqueue(Q *ptr, datatype *data){
	if (q_isfull(ptr)) {
		return -1;
	}
	ptr->tail = (ptr->tail+1)%MAXSIZE;
	ptr->data[ptr->tail] = *data;
	return 0;
}

int q_dequeue(Q *ptr, datatype *data){
	if (q_isempty(ptr)) {
		return -1;
	}
	ptr->head = (ptr->head+1)%MAXSIZE;
	*data = ptr->data[ptr->head];
	return 0;
}

void q_travel(Q *ptr){
	int i;
	if (q_isempty(ptr)) {
		return;
	}
	//for (i = (ptr->head+1)%MAXSIZE;  ; i = (i+1)%MAXSIZE) {
	//	if (i == (ptr->tail+1)%MAXSIZE) {
	//		break;
	//	}
	//	printf("%d ", ptr->data[i]);
	//}
	//printf("\n");

	i = (ptr->head+1) % MAXSIZE;
	while (i != ptr->tail) {
		printf("%d ", ptr->data[i]);
		i = (i+1)%MAXSIZE;
	}
	printf("%d \n", ptr->data[i]);
}


void q_clear(Q *ptr){
	ptr->tail = ptr->head;
}

void q_destroy(Q *ptr){
	free(ptr);
}
