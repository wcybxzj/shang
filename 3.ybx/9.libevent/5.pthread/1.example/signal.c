#include <event3/event-config.h>

#include <sys/types.h>
#ifdef _EVENT_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <sys/queue.h>
#ifdef _EVENT_HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _EVENT_HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>
#ifdef _EVENT_HAVE_FCNTL_H
#include <fcntl.h>
#endif

#include <event3/event.h>
#include <event3/util.h>
#include "log-internal.h"
#include "evthread-internal.h"


#ifndef _EVENT_DISABLE_THREAD_SUPPORT
/* Lock for evsig_base and evsig_base_n_signals_added fields. */
static void *evsig_base_lock = NULL;
#endif




#ifndef _EVENT_DISABLE_THREAD_SUPPORT
int
evsig_global_setup_locks_(const int enable_locks)
{
    EVTHREAD_SETUP_GLOBAL_LOCK(evsig_base_lock, 0);
    return 0;
}
#endif

