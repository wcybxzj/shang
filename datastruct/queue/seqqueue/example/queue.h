#ifndef SEQ_QUEUE_H
#define SEQ_QUEUE_H

#define QUEUEMAX 30 

typedef int	DATA_T; 
typedef enum{OK, FAIL}status_t;

typedef struct  {
	DATA_T *queue;//队列的首地址
	int front;//队头
	int rear;//队尾的下一个位置
}QUEUE;

QUEUE *init_queue(void);

status_t enq_queue(QUEUE *, DATA_T );

status_t deq_queue(QUEUE *, DATA_T *);

status_t empty_queue(QUEUE *);

status_t full_queue(QUEUE *);

void destroy_queue();

#endif
