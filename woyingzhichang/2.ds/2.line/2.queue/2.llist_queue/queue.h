#ifndef QUEUE_H__
#define QUEUE_H__ 
#include "llist.h"
typedef LLIST Q;

Q *q_create(int initsize);

int q_enqueue(Q *ptr, void* data);

int q_dequeue(Q *ptr, void* data);

void q_destroy(Q *ptr);

#endif
