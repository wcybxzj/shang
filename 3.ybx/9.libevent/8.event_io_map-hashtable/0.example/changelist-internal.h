#ifndef _CHANGELIST_H_
#define _CHANGELIST_H_
#include <event3/util.h>

struct event_change {
    /** The fd or signal whose events are to be changed */
    evutil_socket_t fd;
    /* The events that were enabled on the fd before any of these changes
       were made.  May include EV_READ or EV_WRITE. */
    short old_events;

    /* The changes that we want to make in reading and writing on this fd.
     * If this is a signal, then read_change has EV_CHANGE_SIGNAL set,
     * and write_change is unused. */
    ev_uint8_t read_change;
    ev_uint8_t write_change;
};

void event_changelist_init(struct event_changelist *changelist);
void event_changelist_freemem(struct event_changelist *changelist);
#endif
