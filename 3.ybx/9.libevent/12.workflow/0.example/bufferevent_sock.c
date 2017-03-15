#include <sys/types.h>

#include <event3/event-config.h>

#ifdef _EVENT_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _EVENT_HAVE_STDARG_H
#include <stdarg.h>
#endif
#ifdef _EVENT_HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#ifdef _EVENT_HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef _EVENT_HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef _EVENT_HAVE_NETINET_IN6_H
#include <netinet/in6.h>
#endif

#include <event3/util.h>
#include <event3/bufferevent.h>
#include <event3/buffer.h>
#include <event3/bufferevent_struct.h>
#include <event3/bufferevent_compat.h>
#include <event3/event.h>
#include "log-internal.h"
#include "mm-internal.h"
#include "bufferevent-internal.h"
#include "util-internal.h"
#ifdef WIN32
#include "iocp-internal.h"
#endif

static int be_socket_enable(struct bufferevent *, short);
static int be_socket_disable(struct bufferevent *, short);
static void be_socket_destruct(struct bufferevent *);
static int be_socket_adj_timeouts(struct bufferevent *);
static int be_socket_flush(struct bufferevent *, short, enum bufferevent_flush_mode);
static int be_socket_ctrl(struct bufferevent *, enum bufferevent_ctrl_op, union bufferevent_ctrl_data *);

const struct bufferevent_ops bufferevent_ops_socket = {
	"socket",
	evutil_offsetof(struct bufferevent_private, bev),
	be_socket_enable,
	be_socket_disable,
	be_socket_destruct,
	be_socket_adj_timeouts,
	be_socket_flush,
	be_socket_ctrl,
};

#define be_socket_add(ev, t)			\
	_bufferevent_add_event((ev), (t))

static void
bufferevent_socket_outbuf_cb(struct evbuffer *buf,
    const struct evbuffer_cb_info *cbinfo,
    void *arg)
{
	struct bufferevent *bufev = arg;
	struct bufferevent_private *bufev_p =
	    EVUTIL_UPCAST(bufev, struct bufferevent_private, bev);

	if (cbinfo->n_added &&
	    (bufev->enabled & EV_WRITE) &&
	    !event_pending(&bufev->ev_write, EV_WRITE, NULL) &&
	    !bufev_p->write_suspended) {
		/* Somebody added data to the buffer, and we would like to
		 * write, and we were not writing.  So, start writing. */
		if (be_socket_add(&bufev->ev_write, &bufev->timeout_write) == -1) {
		    /* Should we log this? */
		}
	}
}

static void
bufferevent_readcb(evutil_socket_t fd, short event, void *arg)
{
	struct bufferevent *bufev = arg;
	struct bufferevent_private *bufev_p =
	    EVUTIL_UPCAST(bufev, struct bufferevent_private, bev);
	struct evbuffer *input;
	int res = 0;
	short what = BEV_EVENT_READING;
	ev_ssize_t howmuch = -1, readmax=-1;

	_bufferevent_incref_and_lock(bufev);

	if (event == EV_TIMEOUT) {
		what |= BEV_EVENT_TIMEOUT;
		goto error;
	}

	input = bufev->input;

	/*
	 * If we have a high watermark configured then we don't want to
	 * read more data than would make us reach the watermark.
	 */
	if (bufev->wm_read.high != 0) {
		howmuch = bufev->wm_read.high - evbuffer_get_length(input);
		/* we somehow lowered the watermark, stop reading */
		if (howmuch <= 0) {
			bufferevent_wm_suspend_read(bufev);
			goto done;
		}
	}
	readmax = _bufferevent_get_read_max(bufev_p);
	if (howmuch < 0 || howmuch > readmax) /* The use of -1 for "unlimited"
					       * uglifies this code. XXXX */
		howmuch = readmax;
	if (bufev_p->read_suspended)
		goto done;

	evbuffer_unfreeze(input, 0);
	res = evbuffer_read(input, fd, (int)howmuch); /* XXXX evbuffer_read would do better to take and return ev_ssize_t */
	evbuffer_freeze(input, 0);

	if (res == -1) {
		int err = evutil_socket_geterror(fd);
		if (EVUTIL_ERR_RW_RETRIABLE(err))
			goto reschedule;
		/* error case */
		what |= BEV_EVENT_ERROR;
	} else if (res == 0) {
		/* eof case */
		what |= BEV_EVENT_EOF;
	}

	if (res <= 0)
		goto error;

	_bufferevent_decrement_read_buckets(bufev_p, res);

	/* Invoke the user callback - must always be called last */
	if (evbuffer_get_length(input) >= bufev->wm_read.low)
		_bufferevent_run_readcb(bufev);

	goto done;

 reschedule:
	goto done;

 error:
	bufferevent_disable(bufev, EV_READ);
	_bufferevent_run_eventcb(bufev, what);

 done:
	_bufferevent_decref_and_unlock(bufev);
}//end bufferevent_readcb


static void
bufferevent_writecb(evutil_socket_t fd, short event, void *arg)
{
	struct bufferevent *bufev = arg;
	struct bufferevent_private *bufev_p =
	    EVUTIL_UPCAST(bufev, struct bufferevent_private, bev);
	int res = 0;
	short what = BEV_EVENT_WRITING;
	int connected = 0;
	ev_ssize_t atmost = -1;

	_bufferevent_incref_and_lock(bufev);

	if (event == EV_TIMEOUT) {
		what |= BEV_EVENT_TIMEOUT;
		goto error;
	}
	if (bufev_p->connecting) {
		int c = evutil_socket_finished_connecting(fd);
		/* we need to fake the error if the connection was refused
		 * immediately - usually connection to localhost on BSD */
		if (bufev_p->connection_refused) {
		  bufev_p->connection_refused = 0;
		  c = -1;
		}

		if (c == 0)
			goto done;

		bufev_p->connecting = 0;
		if (c < 0) {
			event_del(&bufev->ev_write);
			event_del(&bufev->ev_read);
			_bufferevent_run_eventcb(bufev, BEV_EVENT_ERROR);
			goto done;
		} else {
			connected = 1;
#ifdef WIN32
			if (BEV_IS_ASYNC(bufev)) {
				event_del(&bufev->ev_write);
				bufferevent_async_set_connected(bufev);
				_bufferevent_run_eventcb(bufev,
						BEV_EVENT_CONNECTED);
				goto done;
			}
#endif
			_bufferevent_run_eventcb(bufev,
					BEV_EVENT_CONNECTED);
			if (!(bufev->enabled & EV_WRITE) ||
			    bufev_p->write_suspended) {
				event_del(&bufev->ev_write);
				goto done;
			}
		}
	}

	atmost = _bufferevent_get_write_max(bufev_p);

	if (bufev_p->write_suspended)
		goto done;

	if (evbuffer_get_length(bufev->output)) {
		evbuffer_unfreeze(bufev->output, 1);
		res = evbuffer_write_atmost(bufev->output, fd, atmost);
		evbuffer_freeze(bufev->output, 1);
		if (res == -1) {
			int err = evutil_socket_geterror(fd);
			if (EVUTIL_ERR_RW_RETRIABLE(err))
				goto reschedule;
			what |= BEV_EVENT_ERROR;
		} else if (res == 0) {
			/* eof case
			   XXXX Actually, a 0 on write doesn't indicate
			   an EOF. An ECONNRESET might be more typical.
			 */
			what |= BEV_EVENT_EOF;
		}
		if (res <= 0)
			goto error;

		_bufferevent_decrement_write_buckets(bufev_p, res);
	}

	if (evbuffer_get_length(bufev->output) == 0) {
		event_del(&bufev->ev_write);
	}

	/*
	 * Invoke the user callback if our buffer is drained or below the
	 * low watermark.
	 */
	if ((res || !connected) &&
	    evbuffer_get_length(bufev->output) <= bufev->wm_write.low) {
		_bufferevent_run_writecb(bufev);
	}

	goto done;

 reschedule:
	if (evbuffer_get_length(bufev->output) == 0) {
		event_del(&bufev->ev_write);
	}
	goto done;

 error:
	bufferevent_disable(bufev, EV_WRITE);
	_bufferevent_run_eventcb(bufev, what);

 done:
	_bufferevent_decref_and_unlock(bufev);
}//end bufferevent_writecb


struct bufferevent *
bufferevent_socket_new(struct event_base *base, evutil_socket_t fd,
    int options)
{
	struct bufferevent_private *bufev_p;
	struct bufferevent *bufev;

#ifdef WIN32
	if (base && event_base_get_iocp(base))
		return bufferevent_async_new(base, fd, options);
#endif

	if ((bufev_p = mm_calloc(1, sizeof(struct bufferevent_private)))== NULL)
		return NULL;

	if (bufferevent_init_common(bufev_p, base, &bufferevent_ops_socket,
				    options) < 0) {
		mm_free(bufev_p);
		return NULL;
	}
	bufev = &bufev_p->bev;
	evbuffer_set_flags(bufev->output, EVBUFFER_FLAG_DRAINS_TO_FD);

	event_assign(&bufev->ev_read, bufev->ev_base, fd,
	    EV_READ|EV_PERSIST, bufferevent_readcb, bufev);
	event_assign(&bufev->ev_write, bufev->ev_base, fd,
	    EV_WRITE|EV_PERSIST, bufferevent_writecb, bufev);

	evbuffer_add_cb(bufev->output, bufferevent_socket_outbuf_cb, bufev);

	evbuffer_freeze(bufev->input, 0);
	evbuffer_freeze(bufev->output, 1);

	return bufev;
}// end bufferevent_socket_new

static int
be_socket_enable(struct bufferevent *bufev, short event)
{
	if (event & EV_READ) {
		if (be_socket_add(&bufev->ev_read,&bufev->timeout_read) == -1)
			return -1;
	}
	if (event & EV_WRITE) {
		if (be_socket_add(&bufev->ev_write,&bufev->timeout_write) == -1)
			return -1;
	}
	return 0;
}

static int
be_socket_disable(struct bufferevent *bufev, short event)
{
	struct bufferevent_private *bufev_p =
	    EVUTIL_UPCAST(bufev, struct bufferevent_private, bev);
	if (event & EV_READ) {
		if (event_del(&bufev->ev_read) == -1)
			return -1;
	}
	/* Don't actually disable the write if we are trying to connect. */
	if ((event & EV_WRITE) && ! bufev_p->connecting) {
		if (event_del(&bufev->ev_write) == -1)
			return -1;
	}
	return 0;
}

static void
be_socket_destruct(struct bufferevent *bufev)
{
	struct bufferevent_private *bufev_p =
	    EVUTIL_UPCAST(bufev, struct bufferevent_private, bev);
	evutil_socket_t fd;
	EVUTIL_ASSERT(bufev->be_ops == &bufferevent_ops_socket);

	fd = event_get_fd(&bufev->ev_read);

	event_del(&bufev->ev_read);
	event_del(&bufev->ev_write);

	if ((bufev_p->options & BEV_OPT_CLOSE_ON_FREE) && fd >= 0)
		EVUTIL_CLOSESOCKET(fd);
}

static int
be_socket_adj_timeouts(struct bufferevent *bufev)
{
	int r = 0;
	if (event_pending(&bufev->ev_read, EV_READ, NULL))
		if (be_socket_add(&bufev->ev_read, &bufev->timeout_read) < 0)
			r = -1;
	if (event_pending(&bufev->ev_write, EV_WRITE, NULL)) {
		if (be_socket_add(&bufev->ev_write, &bufev->timeout_write) < 0)
			r = -1;
	}
	return r;
}

static int
be_socket_flush(struct bufferevent *bev, short iotype,
    enum bufferevent_flush_mode mode)
{
	return 0;
}


static void
be_socket_setfd(struct bufferevent *bufev, evutil_socket_t fd)
{
	BEV_LOCK(bufev);
	EVUTIL_ASSERT(bufev->be_ops == &bufferevent_ops_socket);

	event_del(&bufev->ev_read);
	event_del(&bufev->ev_write);

	event_assign(&bufev->ev_read, bufev->ev_base, fd,
	    EV_READ|EV_PERSIST, bufferevent_readcb, bufev);
	event_assign(&bufev->ev_write, bufev->ev_base, fd,
	    EV_WRITE|EV_PERSIST, bufferevent_writecb, bufev);

	if (fd >= 0)
		bufferevent_enable(bufev, bufev->enabled);

	BEV_UNLOCK(bufev);
}

static int
be_socket_ctrl(struct bufferevent *bev, enum bufferevent_ctrl_op op,
    union bufferevent_ctrl_data *data)
{
	switch (op) {
	case BEV_CTRL_SET_FD:
		be_socket_setfd(bev, data->fd);
		return 0;
	case BEV_CTRL_GET_FD:
		data->fd = event_get_fd(&bev->ev_read);
		return 0;
	case BEV_CTRL_GET_UNDERLYING:
	case BEV_CTRL_CANCEL_ALL:
	default:
		return -1;
	}
}
