#ifndef _DEFER_INTERNAL_H_
#define _DEFER_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <event3/event-config.h>
#include <sys/queue.h>

struct deferred_cb;

typedef void (*deferred_cb_fn)(struct deferred_cb *, void *);


struct deferred_cb {
	TAILQ_ENTRY (deferred_cb) cb_next;
	unsigned queued : 1;
	deferred_cb_fn cb;
	void *arg;
};

struct deferred_cb_queue {
	void *lock;
	int active_count;
	void (*notify_fn)(struct deferred_cb_queue *, void *);
	void *notify_arg;
	TAILQ_HEAD (deferred_cb_list, deferred_cb) deferred_cb_list;
};



#ifdef __cplusplus
}
#endif

void event_deferred_cb_queue_init(struct deferred_cb_queue *);
#endif/* _EVENT_INTERNAL_H_ */
