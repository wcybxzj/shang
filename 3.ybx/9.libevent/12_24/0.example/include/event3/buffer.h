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

//查找结构体：
//对于一个数组或者一个文件，只需一个下标或者偏移量就可以定位查找了。
//但对于evbuffer来说，它的数据是由一个个的evbuffer_chain用链表连在一起的。
//所以在evbuffer中定位，不仅仅要有一个偏移量，还要指明是哪个evbuffer_chain，甚至是在evbuffer_chain中的偏移量。
//因此Libevent定义了一个查找(定位)结构体：
struct evbuffer_ptr {
	ev_ssize_t pos;//总偏移量，相对于数据的开始位置

	/* Do not alter the values of fields. */
	struct {
		void *chain;//指明是哪个evbuffer_chain
		size_t pos_in_chain;//在evbuffer_chain中的偏移量
	} _internal;
};

enum evbuffer_ptr_how {  
    EVBUFFER_PTR_SET, //偏移量是一个绝对位置  
    EVBUFFER_PTR_ADD //偏移量是一个相对位置  
};

//设置evbuffer_ptr。evbuffer_ptr_set(buf, &pos, 0, EVBUFFER_PTR_SET)  
//将这个pos指向链表的开头  
//position指明移动的偏移量，how指明该偏移量是绝对偏移量还是相对当前位置的偏移量。 
int evbuffer_ptr_set(struct evbuffer *buffer, struct evbuffer_ptr *ptr,
    size_t position, enum evbuffer_ptr_how how);

//回调函数：
//evbuffer有一个回调函数队列成员callbacks，向evbuffer删除或者添加数据时，就会调用这些回调函数。
//之所以是回调函数队列，是因为一个evbuffer是可以添加多个回调函数的，而且同一个回调函数可以被添加多次。

//使用回调函数时有一点要注意：
//因为当evbuffer被添加或者删除数据时，就会调用这些回调函数，
//所以在回调函数里面不要添加或者删除数据，不然将导致递归，死循环。
//evbuffer的回调函数对bufferevent来说是非常重要的，
//bufferevent的一些重要功能都是基于evbuffer的回调函数完成的。

//回调相关结构体：
struct evbuffer_cb_info {  
    //添加或者删除数据之前的evbuffer有多少字节的数据  
    size_t orig_size;  
    size_t n_added;//添加了多少数据  
    size_t n_deleted;//删除了多少数据  
  
    //因为每次删除或者添加数据都会调用回调函数，所以上面的三个成员只能记录从上一次  
    //回调函数被调用后，到本次回调函数被调用这段时间的情况。  
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

int evbuffer_expand(struct evbuffer *buf, size_t datlen);

int evbuffer_add(struct evbuffer *buf, const void *data, size_t datlen);

int evbuffer_remove(struct evbuffer *buf, void *data, size_t datlen);

ev_ssize_t evbuffer_copyout(struct evbuffer *buf, void *data_out, size_t datlen);

typedef void (*evbuffer_ref_cleanup_cb)(const void *data,
    size_t datalen, void *extra);

int evbuffer_remove_cb_entry(struct evbuffer *buffer,
			     struct evbuffer_cb_entry *ent);

int evbuffer_remove_cb(struct evbuffer *buffer, evbuffer_cb_func cb, void *cbarg);

#define EVBUFFER_CB_ENABLED 1

int evbuffer_cb_set_flags(struct evbuffer *buffer,
			  struct evbuffer_cb_entry *cb, ev_uint32_t flags);

int evbuffer_cb_clear_flags(struct evbuffer *buffer,
			  struct evbuffer_cb_entry *cb, ev_uint32_t flags);

unsigned char *evbuffer_pullup(struct evbuffer *buf, ev_ssize_t size);
int evbuffer_prepend(struct evbuffer *buf, const void *data, size_t size);

int evbuffer_freeze(struct evbuffer *buf, int at_front);
int evbuffer_unfreeze(struct evbuffer *buf, int at_front);

#ifdef __cplusplus
}
#endif

#endif /* _EVENT2_BUFFER_H_ */
