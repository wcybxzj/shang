#ifndef LINK_QUEUE_H
#define LINK_QUEUE_H

#include <llist.h>

typedef LLIST QUEUE;

QUEUE * init_queue(int );

status_t enq_queue(QUEUE *, const void *data);

status_t deq_queue(QUEUE *, void *data);

status_t empty_queue(QUEUE *);

void destroy_queue(QUEUE *);

#endif
