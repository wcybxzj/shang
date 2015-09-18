#include <stdlib.h>

#include "linkqueue.h"

QUEUE * init_queue(int size)
{
	return init_llist(size);
}

status_t enq_queue(QUEUE *ptr, const void *data)
{
	return insert_llist(ptr, data, REAR);
}

static int always(const void *data1, const void *data2)
{
	return 0;
}

status_t deq_queue(QUEUE *ptr, void *data)
{
	return fetch_llist(ptr, NULL, data, always);
}

status_t empty_queue(QUEUE *ptr)
{
	return empty_llist(ptr);
}

void destroy_queue(QUEUE *ptr)
{
	destroy_llist(ptr);
}

