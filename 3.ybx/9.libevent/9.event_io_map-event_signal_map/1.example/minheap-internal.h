#ifndef _MIN_HEAP_H_
#define _MIN_HEAP_H_

#include <event3/event-config.h>
#include <event3/event.h>
#include <event3/event_struct.h>
#include <event3/util.h>
#include "util-internal.h"
#include "mm-internal.h"

typedef struct min_heap
{
	struct event** p;
	unsigned n, a;
} min_heap_t;

#endif //end of  _MIN_HEAP_H_
