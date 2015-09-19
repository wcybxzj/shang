#include <stdlib.h>
#include <string.h>

#include "queue.h"

QUEUE *init_queue(void)
{
	QUEUE *que = NULL;

	que = malloc(sizeof(*que));
	if (NULL == que)
		return NULL;
	que->queue = malloc(sizeof(DATA_T) * QUEUEMAX);
	if (NULL == que->queue)	
		return NULL;
	memset(que->queue, 0x00, sizeof(DATA_T)*QUEUEMAX);

	que->front = que->rear = 0;

	return que;
}

status_t enq_queue(QUEUE *ptr, DATA_T data)
{
	if (full_queue(ptr))
		return FAIL;
	ptr->queue[ptr->rear] = data;	

	ptr->rear = (ptr->rear + 1) % QUEUEMAX;

	return OK;
}

status_t deq_queue(QUEUE *ptr, DATA_T *data)
{
	if (empty_queue(ptr))
		return FAIL;
	*data = ptr->que_data[ptr->front];
	ptr->front = (ptr->front + 1) % QUEUEMAX;
	
	return OK;
}

status_t empty_queue(QUEUE *ptr)
{
	return ptr->front == ptr->rear;
}

status_t full_queue(QUEUE *ptr)
{
	return (ptr->rear+1)%QUEUEMAX == ptr->front;
}

void destroy_queue(QUEUE *ptr)
{
	free(ptr->queue);
	free(ptr);
}

