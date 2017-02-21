#include <event3/event-config.h>

#include <sys/types.h>
#ifdef _EVENT_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <sys/queue.h>
#include <poll.h>
#include <signal.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "event-internal.h"
#include "evsignal-internal.h"
#include "log-internal.h"
#include "evmap-internal.h"
#include <event3/thread.h>
#include "evthread-internal.h"

struct pollop {
	int event_count;		/* Highest number alloc */
	int nfds;			/* Highest number used */
	int realloc_copy;		/* True iff we must realloc
					 * event_set_copy */
	struct pollfd *event_set;
	struct pollfd *event_set_copy;
};
