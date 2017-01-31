#ifndef _EVENT_INTERNAL_H_
#define _EVENT_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <event3/event-config.h>
#include <time.h>
#include <sys/queue.h>
#include <event3/event_struct.h>
#include "mm-internal.h"

//决定是否使用hashtable做struct event_io_map
#ifdef WIN32 
#define EVMAP_USE_HT 
#endif 

#ifdef EVMAP_USE_HT
#include "ht-internal.h"
struct event_map_entry;
HT_HEAD(event_io_map, event_map_entry);
#else
#define event_io_map event_signal_map
#endif

struct event_signal_map {
    /* An array of evmap_io * or of evmap_signal *; empty entries are
     * set to NULL. */
    void **entries;
    /* The number of entries available in entries */
    int nentries;
};


#ifdef __cplusplus
}
#endif
#endif

