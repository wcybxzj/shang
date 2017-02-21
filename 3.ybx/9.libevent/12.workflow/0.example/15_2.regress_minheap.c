#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <event3/util.h>
#include <event3/event.h>
#include <event3/thread.h>

#include "mm-internal.h" //此头本应是libvent内部使用 在这是测试下
#include "log-internal.h" //此头本应是libvent内部使用 在这是测试下
#include "event-internal.h"

static void
set_random_timeout(struct event *ev)
{
	ev->ev_timeout.tv_sec = rand();
	ev->ev_timeout.tv_usec = rand() & 0xfffff;
	ev->ev_timeout_pos.min_heap_idx = -1;
}

static void
check_heap(struct min_heap *heap)
{
	unsigned i;
	//检查除堆顶的所有元素
	for (i = 1; i < heap->n; ++i) {
		unsigned parent_idx = (i-1)/2;
		if(!evutil_timercmp(&heap->p[i]->ev_timeout,
			&heap->p[parent_idx]->ev_timeout, >=)){
			printf("check_heap fail\n");
			exit(1);
		}
	}
}

int main(int argc, const char *argv[])
{
	int i;
	struct min_heap heap;
	struct event *inserted[3];
	struct event *e, *last_e;
	//1.init
	min_heap_ctor(&heap);

	//2.push
	for (i = 0; i < 3; ++i) {
		inserted[i] = malloc(sizeof(struct event));
		set_random_timeout(inserted[i]);
		min_heap_push(&heap, inserted[i]);
	}
	check_heap(&heap);
	assert(min_heap_size(&heap)==3);

	//3.pop
	last_e = min_heap_pop(&heap);
	while (1) {
		e = min_heap_pop(&heap);
		if (!e)
			break;
		if(!evutil_timercmp(&last_e->ev_timeout,
			&e->ev_timeout, <=)){
			printf("min_heap_pop fail\n");
			exit(1);
		}
	}
	assert(min_heap_size(&heap) == 0);


	return 0;
}
