#ifndef _QUEUE_H
#define _QUEUE_H
#include <llist.h>

typedef LLIST QUEUE;

QUEUE *init_queue(int size);

status_t in_queue(QUEUE *myqueue, const void *data);

status_t out_queue(QUEUE *myqueue, void *data);

status_t empty_queue(QUEUE *myqueue);

void destroy_queue(QUEUE *myqueue);

#endif
