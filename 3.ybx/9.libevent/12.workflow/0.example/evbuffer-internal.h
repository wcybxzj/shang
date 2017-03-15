#ifndef _EVBUFFER_INTERNAL_H_
#define _EVBUFFER_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "event2/event-config.h"
#include "event2/util.h"
#include "util-internal.h"
#include "defer-internal.h"

/* Experimental cb flag: "never deferred."  Implementation note:
 * these callbacks may get an inaccurate view of n_del/n_added in their
 * arguments. */
#define EVBUFFER_CB_NODEFER 2

#ifdef WIN32
#include <winsock2.h>
#endif
#include <sys/queue.h>

/* Minimum allocation for a chain.  We define this so that we're burning no
 * more than 5% of each allocation on overhead.  It would be nice to lose even
 * less space, though. */
#if _EVENT_SIZEOF_VOID_P < 8
#define MIN_BUFFER_SIZE	512
#else
#define MIN_BUFFER_SIZE	1024
#endif

struct evbuffer_cb_entry {
	TAILQ_ENTRY(evbuffer_cb_entry) next;
	union {
		evbuffer_cb_func cb_func;
		evbuffer_cb cb_obsolete;
	} cb;
	void *cbarg;
	ev_uint32_t flags;
};

//Libevent将缓冲数据都存放到buffer中。通过一个个的evbuffer_chain连成的链表可以存放很多的缓冲数据。
//这是一个很常见的链表形式。但Libevent有一个很独特的地方，就是那个evbuffer_chain结构体。
//第一:
//该结构体有misalign成员。该成员表示错开不用的buffer空间。
//也就是说buffer中真正的数据是从buffer + misalign开始。
//第二:
//evbuffer_chain结构体buffer是一个指针char*，按道理来说，应该单独调用malloc分配一个堆内存并让buffer指向之。
//但实际上buffer指向的内存和evbuffer_chain结构体本身的存储内存是一起分配的,看下面的EVBUFFER_CHAIN_SIZE 
struct bufferevent;
struct evbuffer_chain;
struct evbuffer {
	struct evbuffer_chain *first;
	struct evbuffer_chain *last;
    //这是一个二级指针。使用*last_with_datap时，指向的是链表中最后一个有数据的evbuffer_chain。  
    //所以last_with_datap存储的是倒数第二个evbuffer_chain的next成员地址。  
    //一开始buffer->last_with_datap = &buffer->first;此时first为NULL。所以当链表没有节点时  
    //*last_with_datap为NULL。当只有一个节点时*last_with_datap就是first。  
	struct evbuffer_chain **last_with_datap;

	size_t total_len;//链表中所有chain的总字节数  
	size_t n_add_for_cb;
	size_t n_del_for_cb;

#ifndef _EVENT_DISABLE_THREAD_SUPPORT
	void *lock;
#endif
	unsigned own_lock : 1;
	unsigned freeze_start : 1;
	unsigned freeze_end : 1;
	unsigned deferred_cbs : 1;
#ifdef WIN32
	unsigned is_overlapped : 1;
#endif
	ev_uint32_t flags;
	struct deferred_cb_queue *cb_queue;
	int refcnt;
	struct deferred_cb deferred;
	TAILQ_HEAD(evbuffer_cb_queue, evbuffer_cb_entry) callbacks;
	struct bufferevent *parent;
};//end of evbuffer


struct evbuffer_chain {
	struct evbuffer_chain *next;
	size_t buffer_len;//buffer的大小
	ev_off_t misalign;//错开不使用的空间。该成员的值一般等于0
    //evbuffer_chain已存数据的字节数 
    //所以要从buffer + misalign + off的位置开始写入数据  
	size_t off;
	unsigned flags;
#define EVBUFFER_MMAP		0x0001	/**< memory in buffer is mmaped */
#define EVBUFFER_SENDFILE	0x0002	/**< a chain used for sendfile */
#define EVBUFFER_REFERENCE	0x0004	/**< a chain with a mem reference */
#define EVBUFFER_IMMUTABLE	0x0008	/**< read-only chain */
#define EVBUFFER_MEM_PINNED_R	0x0010
#define EVBUFFER_MEM_PINNED_W	0x0020
#define EVBUFFER_MEM_PINNED_ANY (EVBUFFER_MEM_PINNED_R|EVBUFFER_MEM_PINNED_W)
#define EVBUFFER_DANGLING	0x0040
	unsigned char *buffer;
};// end evbuffer_chain 


/* this is currently used by both mmap and sendfile */
/* TODO(niels): something strange needs to happen for Windows here, I am not
 * sure what that is, but it needs to get looked into.
 */
struct evbuffer_chain_fd {
	int fd;	/**< the fd associated with this chain */
};


/** callback for a reference buffer; lets us know what to do with it when
 * we're done with it. */
struct evbuffer_chain_reference {
	evbuffer_ref_cleanup_cb cleanupfn;
	void *extra;
};

#define EVBUFFER_CHAIN_SIZE sizeof(struct evbuffer_chain)

#define EVBUFFER_CHAIN_EXTRA(t, c) (t *)((struct evbuffer_chain *)(c) + 1)

#define ASSERT_EVBUFFER_LOCKED(buffer)			\
	EVLOCK_ASSERT_LOCKED((buffer)->lock)


#define EVBUFFER_LOCK(buffer)						\
	do {								\
		EVLOCK_LOCK((buffer)->lock, 0);				\
	} while (0)
#define EVBUFFER_UNLOCK(buffer)						\
	do {								\
		EVLOCK_UNLOCK((buffer)->lock, 0);			\
	} while (0)

void _evbuffer_incref_and_lock(struct evbuffer *buf);

int bufferevent_enable_locking(struct bufferevent *bufev, void *lock);

void _evbuffer_decref_and_unlock(struct evbuffer *buffer);


/** Internal: Given a bufferevent, return its corresponding
 * bufferevent_private. */
#define BEV_UPCAST(b) EVUTIL_UPCAST((b), struct bufferevent_private, bev)

int _evbuffer_expand_fast(struct evbuffer *, size_t, int);

int _evbuffer_read_setup_vecs(struct evbuffer *buf, ev_ssize_t howmuch,
    struct evbuffer_iovec *vecs, int n_vecs, struct evbuffer_chain ***chainp,
    int exact);


void evbuffer_set_parent(struct evbuffer *buf, struct bufferevent *bev);


void evbuffer_invoke_callbacks(struct evbuffer *buf);

#ifdef __cplusplus
}
#endif

#endif /* _EVBUFFER_INTERNAL_H_ */

