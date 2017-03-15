#ifndef _EVENT2_BUFFEREVENT_H_
#define _EVENT2_BUFFEREVENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <event3/event-config.h>
#ifdef _EVENT_HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef _EVENT_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

/* For int types. */
#include <event3/util.h>

#define BEV_EVENT_READING	0x01	/**< error encountered while reading */
#define BEV_EVENT_WRITING	0x02	/**< error encountered while writing */
#define BEV_EVENT_EOF		0x10	/**< eof file reached */
#define BEV_EVENT_ERROR		0x20	/**< unrecoverable error encountered */
#define BEV_EVENT_TIMEOUT	0x40	/**< user-specified timeout reached */
#define BEV_EVENT_CONNECTED	0x80	/**< connect operation finished. */

struct bufferevent
#ifdef _EVENT_IN_DOXYGEN
{}
#endif
;

struct event_base;
struct evbuffer;
struct sockaddr;
typedef void (*bufferevent_data_cb)(struct bufferevent *bev, void *ctx);
typedef void (*bufferevent_event_cb)(struct bufferevent *bev, short what, void *ctx);

/** Options that can be specified when creating a bufferevent */
enum bufferevent_options {
	BEV_OPT_CLOSE_ON_FREE = (1<<0),
	BEV_OPT_THREADSAFE = (1<<1),
	BEV_OPT_DEFER_CALLBACKS = (1<<2),
	BEV_OPT_UNLOCK_CALLBACKS = (1<<3)
};

struct bufferevent *bufferevent_socket_new(struct event_base *base, evutil_socket_t fd, int options);
void bufferevent_free(struct bufferevent *bufev);

void bufferevent_setcb(struct bufferevent *bufev,
		    bufferevent_data_cb readcb, bufferevent_data_cb writecb,
			    bufferevent_event_cb eventcb, void *cbarg);


struct bufferevent *bufferevent_get_underlying(struct bufferevent *bufev);

int bufferevent_write(struct bufferevent *bufev, const void *data, size_t size);

int bufferevent_write_buffer(struct bufferevent *bufev, struct evbuffer *buf);

size_t bufferevent_read(struct bufferevent *bufev, void *data, size_t size);

int bufferevent_read_buffer(struct bufferevent *bufev, struct evbuffer *buf);

int bufferevent_enable(struct bufferevent *bufev, short event);

int bufferevent_disable(struct bufferevent *bufev, short event);

enum bufferevent_flush_mode {
	BEV_NORMAL = 0,
	BEV_FLUSH = 1,
	BEV_FINISHED = 2
};

#ifdef __cplusplus
}
#endif

#endif /* _EVENT2_BUFFEREVENT_H_ */
