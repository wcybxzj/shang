#ifndef _BUFFEREVENT_INTERNAL_H_
#define _BUFFEREVENT_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <event3/event-config.h>
#include <event3/util.h>
#include "defer-internal.h"
#include "evthread-internal.h"
#include <event3/thread.h>
#include "ratelim-internal.h"
#include <event3/bufferevent_struct.h>

#define BEV_SUSPEND_WM 0x01
#define BEV_SUSPEND_BW 0x02
#define BEV_SUSPEND_BW_GROUP 0x04
#define BEV_SUSPEND_LOOKUP 0x08
#define BEV_SUSPEND_FILT_READ 0x10

typedef ev_uint16_t bufferevent_suspend_flags;

struct bufferevent_rate_limit_group {
	TAILQ_HEAD(rlim_group_member_list, bufferevent_private) members;
	struct ev_token_bucket rate_limit;
	struct ev_token_bucket_cfg rate_limit_cfg;
	unsigned read_suspended : 1;
	unsigned write_suspended : 1;
	unsigned pending_unsuspend_read : 1;
	unsigned pending_unsuspend_write : 1;
	ev_uint64_t total_read;
	ev_uint64_t total_written;
	int n_members;
	ev_ssize_t min_share;
	ev_ssize_t configured_min_share;
	struct event master_refill_event;
	void *lock;
};//end bufferevent_rate_limit_group 

struct bufferevent_rate_limit {
	TAILQ_ENTRY(bufferevent_private) next_in_group;
	struct bufferevent_rate_limit_group *group;
	struct ev_token_bucket limit;
	struct ev_token_bucket_cfg *cfg;
	struct event refill_bucket_event;
};

struct bufferevent_private {
	struct bufferevent bev;
	struct evbuffer_cb_entry *read_watermarks_cb;
	//位域技巧只占1字节
	unsigned own_lock : 1;
	unsigned readcb_pending : 1;
	unsigned writecb_pending : 1;
	unsigned connecting : 1;
	unsigned connection_refused : 1;
	short eventcb_pending;
	//位域技巧只占1字节
	bufferevent_suspend_flags read_suspended;
	bufferevent_suspend_flags write_suspended;
	int errno_pending;
	int dns_error;
	struct deferred_cb deferred;
	enum bufferevent_options options;
	int refcnt;
	void *lock;
	struct bufferevent_rate_limit *rate_limiting;
};


/** Possible operations for a control callback. */
enum bufferevent_ctrl_op {
	BEV_CTRL_SET_FD,
	BEV_CTRL_GET_FD,
	BEV_CTRL_GET_UNDERLYING,
	BEV_CTRL_CANCEL_ALL
};

/** Possible data types for a control callback */
union bufferevent_ctrl_data {
	void *ptr;
	evutil_socket_t fd;
};

struct bufferevent_ops {
	const char *type;
	off_t mem_offset;
	int (*enable)(struct bufferevent *, short);
	int (*disable)(struct bufferevent *, short);
	void (*destruct)(struct bufferevent *);
	int (*adj_timeouts)(struct bufferevent *);
	int (*flush)(struct bufferevent *, short, enum bufferevent_flush_mode);
	int (*ctrl)(struct bufferevent *, enum bufferevent_ctrl_op, union bufferevent_ctrl_data *);
};

int bufferevent_init_common(struct bufferevent_private *, struct event_base *, const struct bufferevent_ops *, enum bufferevent_options options);

void bufferevent_suspend_read(struct bufferevent *bufev, bufferevent_suspend_flags what);
void bufferevent_unsuspend_read(struct bufferevent *bufev, bufferevent_suspend_flags what);
void bufferevent_suspend_write(struct bufferevent *bufev, bufferevent_suspend_flags what);
void bufferevent_unsuspend_write(struct bufferevent *bufev, bufferevent_suspend_flags what);

#define bufferevent_wm_suspend_read(b) \
	bufferevent_suspend_read((b), BEV_SUSPEND_WM)
#define bufferevent_wm_unsuspend_read(b) \
	bufferevent_unsuspend_read((b), BEV_SUSPEND_WM)

void bufferevent_incref(struct bufferevent *bufev);

int bufferevent_enable_locking(struct bufferevent *bufev, void *lock);

void _bufferevent_incref_and_lock(struct bufferevent *bufev);
int bufferevent_decref(struct bufferevent *bufev);
int _bufferevent_decref_and_unlock(struct bufferevent *bufev);

void _bufferevent_run_readcb(struct bufferevent *bufev);
void _bufferevent_run_writecb(struct bufferevent *bufev);

void _bufferevent_run_eventcb(struct bufferevent *bufev, short what);

int _bufferevent_add_event(struct event *ev, const struct timeval *tv);

#define BEV_UPCAST(b) EVUTIL_UPCAST((b), struct bufferevent_private, bev)

#ifdef _EVENT_DISABLE_THREAD_SUPPORT
#define BEV_LOCK(b) _EVUTIL_NIL_STMT
#define BEV_UNLOCK(b) _EVUTIL_NIL_STMT
#else
/** Internal: Grab the lock (if any) on a bufferevent */
#define BEV_LOCK(b) do {						\
		struct bufferevent_private *locking =  BEV_UPCAST(b);	\
		EVLOCK_LOCK(locking->lock, 0);				\
	} while (0)

/** Internal: Release the lock (if any) on a bufferevent */
#define BEV_UNLOCK(b) do {						\
		struct bufferevent_private *locking =  BEV_UPCAST(b);	\
		EVLOCK_UNLOCK(locking->lock, 0);			\
	} while (0)
#endif

int _bufferevent_decrement_write_buckets(struct bufferevent_private *bev,
    ev_ssize_t bytes);
int _bufferevent_decrement_read_buckets(struct bufferevent_private *bev,
    ev_ssize_t bytes);

ev_ssize_t _bufferevent_get_read_max(struct bufferevent_private *bev);
ev_ssize_t _bufferevent_get_write_max(struct bufferevent_private *bev);

#ifdef __cplusplus
}
#endif


#endif /* _BUFFEREVENT_INTERNAL_H_ */
