#ifndef _EVENT2_BUFFER_H_
#define _EVENT2_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <event3/event-config.h>
#include <stdarg.h>
#ifdef _EVENT_HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef _EVENT_HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif
#include <event3/util.h>

struct evbuffer
#ifdef _EVENT_IN_DOXYGEN
{}
#endif
;

struct evbuffer_ptr {
	ev_ssize_t pos;

	/* Do not alter the values of fields. */
	struct {
		void *chain;
		size_t pos_in_chain;
	} _internal;
};

struct evbuffer_cb_info {
	size_t orig_size;
	size_t n_added;
	size_t n_deleted;
};

typedef void (*evbuffer_cb_func)(struct evbuffer *buffer, const struct evbuffer_cb_info *info, void *arg);

struct evbuffer_cb_entry;
struct evbuffer_cb_entry *evbuffer_add_cb(struct evbuffer *buffer, evbuffer_cb_func cb, void *cbarg);

#ifdef _EVENT_HAVE_SYS_UIO_H
#define evbuffer_iovec iovec
#define _EVBUFFER_IOVEC_IS_NATIVE
#else
struct evbuffer_iovec {
	void *iov_base;
	size_t iov_len;
};
#endif

struct evbuffer *evbuffer_new(void);

int evbuffer_enable_locking(struct evbuffer *buf, void *lock);


#define EVBUFFER_FLAG_DRAINS_TO_FD 1

int evbuffer_set_flags(struct evbuffer *buf, ev_uint64_t flags);

size_t evbuffer_get_length(const struct evbuffer *buf);


int evbuffer_add(struct evbuffer *buf, const void *data, size_t datlen);

int evbuffer_remove(struct evbuffer *buf, void *data, size_t datlen);

ev_ssize_t evbuffer_copyout(struct evbuffer *buf, void *data_out, size_t datlen);

typedef void (*evbuffer_ref_cleanup_cb)(const void *data,
    size_t datalen, void *extra);

#define EVBUFFER_CB_ENABLED 1

int evbuffer_freeze(struct evbuffer *buf, int at_front);
int evbuffer_unfreeze(struct evbuffer *buf, int at_front);

#ifdef __cplusplus
}
#endif

#endif /* _EVENT2_BUFFER_H_ */
