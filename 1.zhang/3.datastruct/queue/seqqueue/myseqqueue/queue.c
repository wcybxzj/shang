#include <stdio.h>
#include <stdlib.h>
#include <queue.h>
#include <string.h>



QUEUE *init_queue(int size){
	seq_queue *que = NULL;
	//que = malloc(sizeof(*que)+size*QUEUEMAX);//变长结构体使用这行
	que = malloc(sizeof(*que));
	que->que_data = malloc(size * QUEUEMAX) ;//使用变长结构体注释掉这行
	que->size = size;
	que->front = que->rear = 0;
	return que;
}

//入
status_t enq_queue(QUEUE *ptr, const void *data){
	seq_queue *p = ptr;
	if (full_queue(p)) {
		return FAIL;
	}
	memcpy(p->que_data+p->rear, data, p->size);
	p->rear = (p->rear+p->size)%(QUEUEMAX*p->size);
	return OK;
}

//出
status_t deq_queue(QUEUE *ptr, void *data){
	seq_queue *p = ptr;
	if (empty_queue(p)) {
		return FAIL;
	}
	memcpy(data, p->que_data+p->front, p->size);
	p->front = (p->front+p->size)%(QUEUEMAX*p->size);
	return OK;
}

status_t empty_queue(QUEUE *ptr){
	seq_queue *p= ptr;
	return p->rear == p->front;
}

status_t full_queue(QUEUE *ptr){
	seq_queue *p= ptr;
	return (p->rear+p->size)%(QUEUEMAX*p->size) == p->front;
}

void destroy_queue(QUEUE *ptr){
	seq_queue *p= ptr;
	//free(p->que_data);
	free(p);
}
