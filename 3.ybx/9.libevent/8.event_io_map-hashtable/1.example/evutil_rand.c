#include <event3/event-config.h>
#include <limits.h>

#include "util-internal.h"
#include "evthread-internal.h"

#ifdef _EVENT_HAVE_ARC4RANDOM
#include <stdlib.h>
#include <string.h>


#else //_EVENT_HAVE_ARC4RANDOM

#ifndef _EVENT_DISABLE_THREAD_SUPPORT
static void *arc4rand_lock;
#endif

#ifndef _EVENT_DISABLE_THREAD_SUPPORT
int
evutil_secure_rng_global_setup_locks_(const int enable_locks)
{
    EVTHREAD_SETUP_GLOBAL_LOCK(arc4rand_lock, 0);
    return 0;
}
#endif




#endif //end of _EVENT_HAVE_ARC4RANDOM
