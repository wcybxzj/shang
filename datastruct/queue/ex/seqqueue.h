#ifndef _SEQ_QUEUE_H_
#define _SEQ_QUEUE_H_

typedef struct seq_queue_st{
	char *arr;
	int size;
	int num;
	int head;
	int rear;
} SEQ_QUEUE_T; 


QUEUE * init_queue(int );

status_t enq_queue(QUEUE *, const void *data);

status_t deq_queue(QUEUE *, void *data);

status_t empty_queue(QUEUE *);

void destroy_queue(QUEUE *);

#endif

