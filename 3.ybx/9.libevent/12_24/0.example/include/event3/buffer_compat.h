#ifndef _EVENT2_BUFFER_COMPAT_H_
#define _EVENT2_BUFFER_COMPAT_H_

typedef void (*evbuffer_cb)(struct evbuffer *buffer, size_t old_len, size_t new_len, void *arg);

void evbuffer_setcb(struct evbuffer *buffer, evbuffer_cb cb, void *cbarg);

#endif
