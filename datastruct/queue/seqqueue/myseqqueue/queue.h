#ifndef _SEQ_QUEUE_H_
#define _SEQ_QUEUE_H_

typedef enum{OK, FAIL} status_t;

typedef void QUEUE;

#define QUEUEMAX  8

typedef struct {
	int size;//每个保存内容的size
	int front;
	int rear;
	char *que_data;//保存数据 8个字节
	//char que_data[0];//保存数据 0个字节,变长结构体,必须是结构体的最后一项
} seq_queue;

QUEUE *init_queue(int size);

status_t enq_queue(QUEUE *ptr, const void *data);

status_t deq_queue(QUEUE *ptr, void *data);

int len_queue(QUEUE *ptr);

status_t empty_queue(QUEUE *ptr);

status_t full_queue(QUEUE *ptr);

void destroy_queue();


#endif
