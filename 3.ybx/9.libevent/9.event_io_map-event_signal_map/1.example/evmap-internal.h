#ifndef _EVMAP_H_
#define _EVMAP_H_

/** @file evmap-internal.h
 *  *
 *   * An event_map is a utility structure to map each fd or signal to zero or
 *    * more events.  Functions to manipulate event_maps should only be used from
 *     * inside libevent.  They generally need to hold the lock on the corresponding
 *      * event_base.
 *       **/

struct event_base;
struct event;

/** Initialize an event_map for use.
 *  */
void evmap_io_initmap(struct event_io_map* ctx);
int evmap_signal_add(struct event_base *base, int signum, struct event *ev);

#endif
