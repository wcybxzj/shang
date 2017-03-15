#include <sys/types.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

#include <event3/event.h>
#include <event3/event_struct.h>
#include <event3/util.h>
#include <event3/bufferevent.h>
#include <event3/bufferevent_struct.h>
#include <event3/buffer.h>

#include "ratelim-internal.h"

#include "bufferevent-internal.h"
#include "mm-internal.h"
#include "util-internal.h"
#include "event-internal.h"

int
ev_token_bucket_update(struct ev_token_bucket *bucket,
    const struct ev_token_bucket_cfg *cfg,
    ev_uint32_t current_tick)
{
	/* It's okay if the tick number overflows, since we'll just
	 * wrap around when we do the unsigned substraction. */
	unsigned n_ticks = current_tick - bucket->last_updated;

	/* Make sure some ticks actually happened, and that time didn't
	 * roll back. */
	if (n_ticks == 0 || n_ticks > INT_MAX)
		return 0;

	/* Naively, we would say
		bucket->limit += n_ticks * cfg->rate;

		if (bucket->limit > cfg->maximum)
			bucket->limit = cfg->maximum;

	   But we're worried about overflow, so we do it like this:
	*/

	if ((cfg->read_maximum - bucket->read_limit) / n_ticks < cfg->read_rate)
		bucket->read_limit = cfg->read_maximum;
	else
		bucket->read_limit += n_ticks * cfg->read_rate;


	if ((cfg->write_maximum - bucket->write_limit) / n_ticks < cfg->write_rate)
		bucket->write_limit = cfg->write_maximum;
	else
		bucket->write_limit += n_ticks * cfg->write_rate;


	bucket->last_updated = current_tick;

	return 1;
}//end ev_token_bucket_update


#define MAX_TO_READ_EVER 16384
#define MAX_TO_WRITE_EVER 16384

#define LOCK_GROUP(g) EVLOCK_LOCK((g)->lock, 0)
#define UNLOCK_GROUP(g) EVLOCK_UNLOCK((g)->lock, 0)

static int _bev_group_suspend_reading(struct bufferevent_rate_limit_group *g);
static int _bev_group_suspend_writing(struct bufferevent_rate_limit_group *g);
static void _bev_group_unsuspend_reading(struct bufferevent_rate_limit_group *g);
static void _bev_group_unsuspend_writing(struct bufferevent_rate_limit_group *g);

static inline void
bufferevent_update_buckets(struct bufferevent_private *bev)
{
	/* Must hold lock on bev. */
	struct timeval now;
	unsigned tick;
	event_base_gettimeofday_cached(bev->bev.ev_base, &now);
	tick = ev_token_bucket_get_tick(&now, bev->rate_limiting->cfg);
	if (tick != bev->rate_limiting->limit.last_updated)
		ev_token_bucket_update(&bev->rate_limiting->limit,
		    bev->rate_limiting->cfg, tick);
}

ev_uint32_t
ev_token_bucket_get_tick(const struct timeval *tv,
    const struct ev_token_bucket_cfg *cfg)
{
	ev_uint64_t msec = (ev_uint64_t)tv->tv_sec * 1000 + tv->tv_usec / 1000;
	return (unsigned)(msec / cfg->msec_per_tick);
}

static inline ev_ssize_t
_bufferevent_get_rlim_max(struct bufferevent_private *bev, int is_write)
{
	ev_ssize_t max_so_far = is_write?MAX_TO_WRITE_EVER:MAX_TO_READ_EVER;

#define LIM(x)						\
	(is_write ? (x).write_limit : (x).read_limit)

#define GROUP_SUSPENDED(g)			\
	(is_write ? (g)->write_suspended : (g)->read_suspended)

#define CLAMPTO(x)				\
	do {					\
		if (max_so_far > (x))		\
			max_so_far = (x);	\
	} while (0);

	if (!bev->rate_limiting)
		return max_so_far;

	if (bev->rate_limiting->cfg) {
		bufferevent_update_buckets(bev);
		max_so_far = LIM(bev->rate_limiting->limit);
	}
	if (bev->rate_limiting->group) {
		struct bufferevent_rate_limit_group *g =
		    bev->rate_limiting->group;
		ev_ssize_t share;
		LOCK_GROUP(g);
		if (GROUP_SUSPENDED(g)) {
			/* We can get here if we failed to lock this
			 * particular bufferevent while suspending the whole
			 * group. */
			if (is_write)
				bufferevent_suspend_write(&bev->bev,
				    BEV_SUSPEND_BW_GROUP);
			else
				bufferevent_suspend_read(&bev->bev,
				    BEV_SUSPEND_BW_GROUP);
			share = 0;
		} else {
			/* XXXX probably we should divide among the active
			 * members, not the total members. */
			share = LIM(g->rate_limit) / g->n_members;
			if (share < g->min_share)
				share = g->min_share;
		}
		UNLOCK_GROUP(g);
		CLAMPTO(share);
	}

	if (max_so_far < 0)
		max_so_far = 0;
	return max_so_far;
}//end _bufferevent_get_rlim_max

ev_ssize_t
_bufferevent_get_read_max(struct bufferevent_private *bev)
{
	return _bufferevent_get_rlim_max(bev, 0);
}

ev_ssize_t
_bufferevent_get_write_max(struct bufferevent_private *bev)
{
	return _bufferevent_get_rlim_max(bev, 1);
}

int
_bufferevent_decrement_read_buckets(struct bufferevent_private *bev, ev_ssize_t bytes)
{
	/* XXXXX Make sure all users of this function check its return value */
	int r = 0;
	/* need to hold lock on bev */
	if (!bev->rate_limiting)
		return 0;

	if (bev->rate_limiting->cfg) {
		bev->rate_limiting->limit.read_limit -= bytes;
		if (bev->rate_limiting->limit.read_limit <= 0) {
			bufferevent_suspend_read(&bev->bev, BEV_SUSPEND_BW);
			if (event_add(&bev->rate_limiting->refill_bucket_event,
				&bev->rate_limiting->cfg->tick_timeout) < 0)
				r = -1;
		} else if (bev->read_suspended & BEV_SUSPEND_BW) {
			if (!(bev->write_suspended & BEV_SUSPEND_BW))
				event_del(&bev->rate_limiting->refill_bucket_event);
			bufferevent_unsuspend_read(&bev->bev, BEV_SUSPEND_BW);
		}
	}

	if (bev->rate_limiting->group) {
		LOCK_GROUP(bev->rate_limiting->group);
		bev->rate_limiting->group->rate_limit.read_limit -= bytes;
		bev->rate_limiting->group->total_read += bytes;
		if (bev->rate_limiting->group->rate_limit.read_limit <= 0) {
			_bev_group_suspend_reading(bev->rate_limiting->group);
		} else if (bev->rate_limiting->group->read_suspended) {
			_bev_group_unsuspend_reading(bev->rate_limiting->group);
		}
		UNLOCK_GROUP(bev->rate_limiting->group);
	}

	return r;
}//end _bufferevent_decrement_read_buckets


int
_bufferevent_decrement_write_buckets(struct bufferevent_private *bev, ev_ssize_t bytes)
{
	/* XXXXX Make sure all users of this function check its return value */
	int r = 0;
	/* need to hold lock */
	if (!bev->rate_limiting)
		return 0;

	if (bev->rate_limiting->cfg) {
		bev->rate_limiting->limit.write_limit -= bytes;
		if (bev->rate_limiting->limit.write_limit <= 0) {
			bufferevent_suspend_write(&bev->bev, BEV_SUSPEND_BW);
			if (event_add(&bev->rate_limiting->refill_bucket_event,
				&bev->rate_limiting->cfg->tick_timeout) < 0)
				r = -1;
		} else if (bev->write_suspended & BEV_SUSPEND_BW) {
			if (!(bev->read_suspended & BEV_SUSPEND_BW))
				event_del(&bev->rate_limiting->refill_bucket_event);
			bufferevent_unsuspend_write(&bev->bev, BEV_SUSPEND_BW);
		}
	}

	if (bev->rate_limiting->group) {
		LOCK_GROUP(bev->rate_limiting->group);
		bev->rate_limiting->group->rate_limit.write_limit -= bytes;
		bev->rate_limiting->group->total_written += bytes;
		if (bev->rate_limiting->group->rate_limit.write_limit <= 0) {
			_bev_group_suspend_writing(bev->rate_limiting->group);
		} else if (bev->rate_limiting->group->write_suspended) {
			_bev_group_unsuspend_writing(bev->rate_limiting->group);
		}
		UNLOCK_GROUP(bev->rate_limiting->group);
	}

	return r;
}//end _bufferevent_decrement_write_buckets

static int
_bev_group_suspend_reading(struct bufferevent_rate_limit_group *g)
{
	/* Needs group lock */
	struct bufferevent_private *bev;
	g->read_suspended = 1;
	g->pending_unsuspend_read = 0;

	TAILQ_FOREACH(bev, &g->members, rate_limiting->next_in_group) {
		if (EVLOCK_TRY_LOCK(bev->lock)) {
			bufferevent_suspend_read(&bev->bev,
			    BEV_SUSPEND_BW_GROUP);
			EVLOCK_UNLOCK(bev->lock, 0);
		}
	}
	return 0;
}//end _bev_group_suspend_reading


static int
_bev_group_suspend_writing(struct bufferevent_rate_limit_group *g)
{
	struct bufferevent_private *bev;
	g->write_suspended = 1;
	g->pending_unsuspend_write = 0;
	TAILQ_FOREACH(bev, &g->members, rate_limiting->next_in_group) {
		if (EVLOCK_TRY_LOCK(bev->lock)) {
			bufferevent_suspend_write(&bev->bev,
			    BEV_SUSPEND_BW_GROUP);
			EVLOCK_UNLOCK(bev->lock, 0);
		}
	}
	return 0;
}//end _bev_group_suspend_writing

static struct bufferevent_private *
_bev_group_random_element(struct bufferevent_rate_limit_group *group)
{
	int which;
	struct bufferevent_private *bev;

	/* requires group lock */

	if (!group->n_members)
		return NULL;

	EVUTIL_ASSERT(! TAILQ_EMPTY(&group->members));

	which = _evutil_weakrand() % group->n_members;

	bev = TAILQ_FIRST(&group->members);
	while (which--)
		bev = TAILQ_NEXT(bev, rate_limiting->next_in_group);

	return bev;
}

#define FOREACH_RANDOM_ORDER(block)			\
	do {						\
		first = _bev_group_random_element(g);	\
		for (bev = first; bev != TAILQ_END(&g->members); \
		    bev = TAILQ_NEXT(bev, rate_limiting->next_in_group)) { \
			block ;					 \
		}						 \
		for (bev = TAILQ_FIRST(&g->members); bev && bev != first; \
		    bev = TAILQ_NEXT(bev, rate_limiting->next_in_group)) { \
			block ;						\
		}							\
	} while (0)


static void
_bev_group_unsuspend_reading(struct bufferevent_rate_limit_group *g)
{
	int again = 0;
	struct bufferevent_private *bev, *first;

	g->read_suspended = 0;
	FOREACH_RANDOM_ORDER({
		if (EVLOCK_TRY_LOCK(bev->lock)) {
			bufferevent_unsuspend_read(&bev->bev,
			    BEV_SUSPEND_BW_GROUP);
			EVLOCK_UNLOCK(bev->lock, 0);
		} else {
			again = 1;
		}
	});
	g->pending_unsuspend_read = again;
}//end _bev_group_unsuspend_reading

static void
_bev_group_unsuspend_writing(struct bufferevent_rate_limit_group *g)
{
	int again = 0;
	struct bufferevent_private *bev, *first;
	g->write_suspended = 0;

	FOREACH_RANDOM_ORDER({
		if (EVLOCK_TRY_LOCK(bev->lock)) {
			bufferevent_unsuspend_write(&bev->bev,
			    BEV_SUSPEND_BW_GROUP);
			EVLOCK_UNLOCK(bev->lock, 0);
		} else {
			again = 1;
		}
	});
	g->pending_unsuspend_write = again;
}//end _bev_group_unsuspend_writing

int
bufferevent_remove_from_rate_limit_group_internal(struct bufferevent *bev,
    int unsuspend)
{
	struct bufferevent_private *bevp =
	    EVUTIL_UPCAST(bev, struct bufferevent_private, bev);
	BEV_LOCK(bev);
	if (bevp->rate_limiting && bevp->rate_limiting->group) {
		struct bufferevent_rate_limit_group *g =
		    bevp->rate_limiting->group;
		LOCK_GROUP(g);
		bevp->rate_limiting->group = NULL;
		--g->n_members;
		TAILQ_REMOVE(&g->members, bevp, rate_limiting->next_in_group);
		UNLOCK_GROUP(g);
	}
	if (unsuspend) {
		bufferevent_unsuspend_read(bev, BEV_SUSPEND_BW_GROUP);
		bufferevent_unsuspend_write(bev, BEV_SUSPEND_BW_GROUP);
	}
	BEV_UNLOCK(bev);
	return 0;
}

