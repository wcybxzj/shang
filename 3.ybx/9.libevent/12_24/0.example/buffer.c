#include <event3/event-config.h>

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#include <io.h>
#endif

#ifdef _EVENT_HAVE_VASPRINTF
/* If we have vasprintf, we need to define this before we include stdio.h. */
#define _GNU_SOURCE
#endif

#include <sys/types.h>

#ifdef _EVENT_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef _EVENT_HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef _EVENT_HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif

#ifdef _EVENT_HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef _EVENT_HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif

#ifdef _EVENT_HAVE_SYS_SENDFILE_H
#include <sys/sendfile.h>
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
#include <limits.h>

#include <event3/event.h>
#include <event3/buffer.h>
#include <event3/buffer_compat.h>
#include <event3/bufferevent.h>
#include <event3/bufferevent_compat.h>
#include <event3/bufferevent_struct.h>
#include <event3/thread.h>
#include <event3/event-config.h>
#include "log-internal.h"
#include "mm-internal.h"
#include "util-internal.h"
#include "evthread-internal.h"
#include "evbuffer-internal.h"
#include "bufferevent-internal.h"

/* Mask of user-selectable callback flags. */
#define EVBUFFER_CB_USER_FLAGS	    0xffff
/* Mask of all internal-use-only flags. */
#define EVBUFFER_CB_INTERNAL_FLAGS  0xffff0000
/* Flag set if the callback is using the cb_obsolete function pointer  */
#define EVBUFFER_CB_OBSOLETE	       0x00040000


/* evbuffer_chain support */
#define CHAIN_SPACE_PTR(ch) ((ch)->buffer + (ch)->misalign + (ch)->off)
//计算evbuffer_chain的可用空间是多少  
#define CHAIN_SPACE_LEN(ch) ((ch)->flags & EVBUFFER_IMMUTABLE ? \
	    0 : (ch)->buffer_len - ((ch)->misalign + (ch)->off))

#define CHAIN_PINNED(ch)  (((ch)->flags & EVBUFFER_MEM_PINNED_ANY) != 0)
#define CHAIN_PINNED_R(ch)  (((ch)->flags & EVBUFFER_MEM_PINNED_R) != 0)

static void evbuffer_chain_align(struct evbuffer_chain *chain);
static int evbuffer_chain_should_realign(struct evbuffer_chain *chain,
    size_t datalen);
static void evbuffer_deferred_callback(struct deferred_cb *cb, void *arg);
static int evbuffer_ptr_memcmp(const struct evbuffer *buf,
    const struct evbuffer_ptr *pos, const char *mem, size_t len);
static struct evbuffer_chain *evbuffer_expand_singlechain(struct evbuffer *buf,
    size_t datlen);

//参考73.flexible_array/3.Flexible_Array.c/test3()
//size是buffer所需的大小  
static struct evbuffer_chain *
evbuffer_chain_new(size_t size)
{
	struct evbuffer_chain *chain;
	size_t to_alloc;

    //所需的大小size 再加上evbuffer_chain结构体本身所需的内存大小。
	//这样做的原因是，evbuffer_chain本身是管理  
    //buffer的结构体。但buffer内存就分配在evbuffer_chain结构体存储  
    //内存的后面。所以要申请多一些内存。  
	size += EVBUFFER_CHAIN_SIZE;

	/* get the next largest memory that can hold the buffer */
	to_alloc = MIN_BUFFER_SIZE;//1024  //内存块的最小值  
	while (to_alloc < size)
		to_alloc <<= 1;

	//从分配的内存大小可以知道，evbuffer_chain结构体和buffer是一起分配的  
    //也就是说他们是存放在同一块内存中  
	/* we get everything in one chunk */
	if ((chain = mm_malloc(to_alloc)) == NULL)
		return (NULL);

    //只需初始化最前面的结构体部分即可 
	memset(chain, 0, EVBUFFER_CHAIN_SIZE);

	//buffer_len存储的是buffer的大小 
	chain->buffer_len = to_alloc - EVBUFFER_CHAIN_SIZE;

     //宏的作用就是返回，chain + sizeof(evbuffer_chain) 的内存地址。  
     //其效果就是buffer指向的内存刚好是在evbuffer_chain的后面。 
	/* this way we can manipulate the buffer to different addresses,
	 * which is required for mmap for example.
	 */
	chain->buffer = EVBUFFER_CHAIN_EXTRA(u_char, chain);

	return (chain);
}//end evbuffer_chain_new

static inline void
evbuffer_chain_free(struct evbuffer_chain *chain)
{
	if (CHAIN_PINNED(chain)) {
		chain->flags |= EVBUFFER_DANGLING;
		return;
	}
	if (chain->flags & (EVBUFFER_MMAP|EVBUFFER_SENDFILE|
		EVBUFFER_REFERENCE)) {
		if (chain->flags & EVBUFFER_REFERENCE) {
			struct evbuffer_chain_reference *info =
			    EVBUFFER_CHAIN_EXTRA(
				    struct evbuffer_chain_reference,
				    chain);
			if (info->cleanupfn)
				(*info->cleanupfn)(chain->buffer,
				    chain->buffer_len,
				    info->extra);
		}
#ifdef _EVENT_HAVE_MMAP
		if (chain->flags & EVBUFFER_MMAP) {
			struct evbuffer_chain_fd *info =
			    EVBUFFER_CHAIN_EXTRA(struct evbuffer_chain_fd,
				chain);
			if (munmap(chain->buffer, chain->buffer_len) == -1)
				event_warn("%s: munmap failed", __func__);
			if (close(info->fd) == -1)
				event_warn("%s: close(%d) failed",
				    __func__, info->fd);
		}
#endif
#ifdef USE_SENDFILE
		if (chain->flags & EVBUFFER_SENDFILE) {
			struct evbuffer_chain_fd *info =
			    EVBUFFER_CHAIN_EXTRA(struct evbuffer_chain_fd,
				chain);
			if (close(info->fd) == -1)
				event_warn("%s: close(%d) failed",
				    __func__, info->fd);
		}
#endif
	}

	mm_free(chain);
}//end evbuffer_chain_free

static void
evbuffer_free_all_chains(struct evbuffer_chain *chain)
{
	struct evbuffer_chain *next;
	for (; chain; chain = next) {
		next = chain->next;
		evbuffer_chain_free(chain);
	}
}

#ifndef NDEBUG
static int
evbuffer_chains_all_empty(struct evbuffer_chain *chain)
{
	for (; chain; chain = chain->next) {
		if (chain->off)
			return 0;
	}
	return 1;
}
#else
static inline int evbuffer_chains_all_empty(struct evbuffer_chain *chain) {
	return 1;
}
#endif

static void
evbuffer_chain_insert(struct evbuffer *buf,
    struct evbuffer_chain *chain)
{
	ASSERT_EVBUFFER_LOCKED(buf);
	//新建evbuffer时是把整个evbuffer结构体都赋值0，  
    //并有buffer->last_with_datap = &buffer->first;  
    //所以*buf->last_with_datap就是first的值，所以一开始为NULL  
	if (*buf->last_with_datap == NULL) {
		EVUTIL_ASSERT(buf->last_with_datap == &buf->first);
		EVUTIL_ASSERT(buf->first == NULL);
		buf->first = buf->last = chain;
	} else {
		struct evbuffer_chain **ch = buf->last_with_datap;
        /* Find the first victim chain.  It might be *last_with_datap */  
        //(*ch)->off != 0表示该evbuffer_chain有数据了  
        //CHAIN_PINNED(*ch)则表示该evbuffer_chain不能被修改  
        //在链表中寻找到一个可以使用的evbuffer_chain.  
        //可以使用是指该chain没有数据并且可以修改。 
		while ((*ch) && ((*ch)->off != 0 || CHAIN_PINNED(*ch)))
			ch = &(*ch)->next;//取的还是next地址。 这样看&((*ch)->next)更清晰

		//在已有的链表中找不到一个满足条件的evbuffer_chain。一般都是这种情况
		if (*ch == NULL) {
			buf->last->next = chain;
			//要插入的这个chain是有数据的
			if (chain->off){
				//last_with_datap指向的是倒数第二个有数据的chain的next  
				buf->last_with_datap = &buf->last->next;
			}
		} else {
			//断言，从这个节点开始，后面的说有节点都是没有数据的
			EVUTIL_ASSERT(evbuffer_chains_all_empty(*ch));
			//释放从这个节点开始的余下链表节点
			evbuffer_free_all_chains(*ch);
			//把这个chain插入到最后
			*ch = chain;
		}
		buf->last = chain;
	}
	buf->total_len += chain->off;
}//end evbuffer_chain_insert

static inline struct evbuffer_chain *
evbuffer_chain_insert_new(struct evbuffer *buf, size_t datlen)
{
	struct evbuffer_chain *chain;
	if ((chain = evbuffer_chain_new(datlen)) == NULL)
		return NULL;
	evbuffer_chain_insert(buf, chain);
	return chain;
}

struct evbuffer *
evbuffer_new(void)
{
	struct evbuffer *buffer;

	buffer = mm_calloc(1, sizeof(struct evbuffer));
	if (buffer == NULL)
		return (NULL);

	TAILQ_INIT(&buffer->callbacks);
	buffer->refcnt = 1;
	buffer->last_with_datap = &buffer->first;

	return (buffer);
}

int
evbuffer_set_flags(struct evbuffer *buf, ev_uint64_t flags)
{
	EVBUFFER_LOCK(buf);
	buf->flags |= (ev_uint32_t)flags;
	EVBUFFER_UNLOCK(buf);
	return 0;
}

void
_evbuffer_incref_and_lock(struct evbuffer *buf)
{
	EVBUFFER_LOCK(buf);
	++buf->refcnt;
}

//锁操作：
//在前一篇博文可以看到很多函数在操作前都需要对这个evbuffer进行加锁。
//同event_base不同，如果evbuffer支持锁的话，要显式地调用函数evbuffer_enable_locking。
//
//可以看到，第二个参数可以为NULL。
//此时函数内部会申请一个锁。
//明显如果要让evbuffer能使用锁，就必须在一开始就调用evthread_use_pthreads()
//参数可以是一个锁变量也可以是NULL
int
evbuffer_enable_locking(struct evbuffer *buf, void *lock)
{
#ifdef _EVENT_DISABLE_THREAD_SUPPORT
	return -1;
#else
	if (buf->lock)
		return -1;

	if (!lock) {
		//自己分配锁变量 
		EVTHREAD_ALLOC_LOCK(lock, EVTHREAD_LOCKTYPE_RECURSIVE);
		if (!lock)
			return -1;
		buf->lock = lock;
		//该evbuffer拥有锁，到时需要释放锁内存
		buf->own_lock = 1;
	} else {
		buf->lock = lock;//使用参数提供的锁 
		buf->own_lock = 0;//自己没有拥有锁。不需要释放锁内存 
	}

	return 0;
#endif
}

void
evbuffer_set_parent(struct evbuffer *buf, struct bufferevent *bev)
{
	EVBUFFER_LOCK(buf);
	buf->parent = bev;
	EVBUFFER_UNLOCK(buf);
}

static void
evbuffer_remove_all_callbacks(struct evbuffer *buffer)
{
	struct evbuffer_cb_entry *cbent;

	while ((cbent = TAILQ_FIRST(&buffer->callbacks))) {
	    TAILQ_REMOVE(&buffer->callbacks, cbent, next);
	    mm_free(cbent);
	}
}

void
_evbuffer_decref_and_unlock(struct evbuffer *buffer)
{
	struct evbuffer_chain *chain, *next;
	ASSERT_EVBUFFER_LOCKED(buffer);

	EVUTIL_ASSERT(buffer->refcnt > 0);

	if (--buffer->refcnt > 0) {
		EVBUFFER_UNLOCK(buffer);
		return;
	}

	for (chain = buffer->first; chain != NULL; chain = next) {
		next = chain->next;
		evbuffer_chain_free(chain);
	}
	evbuffer_remove_all_callbacks(buffer);
	if (buffer->deferred_cbs)
		event_deferred_cb_cancel(buffer->cb_queue, &buffer->deferred);

	EVBUFFER_UNLOCK(buffer);
	if (buffer->own_lock)
		EVTHREAD_FREE_LOCK(buffer->lock, EVTHREAD_LOCKTYPE_RECURSIVE);
	mm_free(buffer);
}

void
evbuffer_free(struct evbuffer *buffer)
{
	EVBUFFER_LOCK(buffer);
	_evbuffer_decref_and_unlock(buffer);
}
void
evbuffer_lock(struct evbuffer *buf)
{
	EVBUFFER_LOCK(buf);
}

void
evbuffer_unlock(struct evbuffer *buf)
{
	EVBUFFER_UNLOCK(buf);
}

size_t
evbuffer_get_length(const struct evbuffer *buffer)
{
	size_t result;

	EVBUFFER_LOCK(buffer);

	result = (buffer->total_len);

	EVBUFFER_UNLOCK(buffer);

	return result;
}

static int
advance_last_with_data(struct evbuffer *buf)
{
	int n = 0;
	ASSERT_EVBUFFER_LOCKED(buf);

	if (!*buf->last_with_datap)
		return 0;

	while ((*buf->last_with_datap)->next && (*buf->last_with_datap)->next->off) {
		buf->last_with_datap = &(*buf->last_with_datap)->next;
		++n;
	}
	return n;
}


static inline int
HAS_PINNED_R(struct evbuffer *buf)
{
	return (buf->last && CHAIN_PINNED_R(buf->last));
}

static inline void
ZERO_CHAIN(struct evbuffer *dst)
{
	ASSERT_EVBUFFER_LOCKED(dst);
	dst->first = NULL;
	dst->last = NULL;
	dst->last_with_datap = &(dst)->first;
	dst->total_len = 0;
}

//有时我们不仅仅想复制数据，还想删除数据，或者是复制后就删除数据。
//这些操作在socket编程中还是很常见的。
int
evbuffer_drain(struct evbuffer *buf, size_t len)
{
	struct evbuffer_chain *chain, *next;
	size_t remaining, old_len;
	int result = 0;

	EVBUFFER_LOCK(buf);
	old_len = buf->total_len;

	if (old_len == 0)
		goto done;
	//冻结缓冲区头部，禁止删除头部数据
	if (buf->freeze_start) {
		result = -1;
		goto done;
	}

	//要删除的数据量大于等于已有的数据量。并且这个evbuffer是可以删除的
	if (len >= old_len && !HAS_PINNED_R(buf)) {
		len = old_len;
		for (chain = buf->first; chain != NULL; chain = next) {
			next = chain->next;
			evbuffer_chain_free(chain);
		}

		ZERO_CHAIN(buf);//相当于初试化evbuffer的链表
	} else {
		if (len >= old_len)
			len = old_len;

		buf->total_len -= len;
		remaining = len;
		for (chain = buf->first;
		     remaining >= chain->off;
		     chain = next) {
			next = chain->next;
			remaining -= chain->off;

            //已经删除到最后一个有数据的evbuffer_chain了  
			if (chain == *buf->last_with_datap) {
				buf->last_with_datap = &buf->first;
			}

            //删除到倒数第二个有数据的evbuffer_chain  
			if (&chain->next == buf->last_with_datap)
				buf->last_with_datap = &buf->first;

            //这个chain被固定了，不能删除  
			if (CHAIN_PINNED_R(chain)) {
				EVUTIL_ASSERT(remaining == 0);
				chain->misalign += chain->off;
				chain->off = 0;
				break;////后面的evbuffer_chain也是固定的
			} else
				evbuffer_chain_free(chain);
		}

		buf->first = chain;
		if (chain) {
			chain->misalign += remaining;
			chain->off -= remaining;
		}
	}

	buf->n_del_for_cb += len;
	/* Tell someone about changes in this buffer */
	evbuffer_invoke_callbacks(buf);//因为删除数据，所以也要调用回调函数

done:
	EVBUFFER_UNLOCK(buf);
	return result;
}//end evbuffer_drain


//可以看到evbuffer_remove是先复制数据，然后才删除evbuffer的数据。
//而evbuffer_drain则直接删除evbuffer的数据，而不会复制。
/* Reads data from an event buffer and drains the bytes read */
int
evbuffer_remove(struct evbuffer *buf, void *data_out, size_t datlen)
{
	ev_ssize_t n;
	EVBUFFER_LOCK(buf);
	n = evbuffer_copyout(buf, data_out, datlen);
	if (n > 0) {
		if (evbuffer_drain(buf, n)<0)
			n = -1;
	}
	EVBUFFER_UNLOCK(buf);
	return (int)n;
}

ev_ssize_t
evbuffer_copyout(struct evbuffer *buf, void *data_out, size_t datlen)
{
	/*XXX fails badly on sendfile case. */
	struct evbuffer_chain *chain;
	char *data = data_out;
	size_t nread;
	ev_ssize_t result = 0;

	EVBUFFER_LOCK(buf);

	chain = buf->first;

	if (datlen >= buf->total_len)
		datlen = buf->total_len;

	if (datlen == 0)
		goto done;

	if (buf->freeze_start) {
		result = -1;
		goto done;
	}

	nread = datlen;

	while (datlen && datlen >= chain->off) {
		memcpy(data, chain->buffer + chain->misalign, chain->off);
		data += chain->off;
		datlen -= chain->off;

		chain = chain->next;
		EVUTIL_ASSERT(chain || datlen==0);
	}

	if (datlen) {
		EVUTIL_ASSERT(chain);
		memcpy(data, chain->buffer + chain->misalign, datlen);
	}

	result = nread;
done:
	EVBUFFER_UNLOCK(buf);
	return result;
}//end evbuffer_copyout



unsigned char *
evbuffer_pullup(struct evbuffer *buf, ev_ssize_t size)
{
	struct evbuffer_chain *chain, *next, *tmp, *last_with_data;
	unsigned char *buffer, *result = NULL;
	ev_ssize_t remaining;
	int removed_last_with_data = 0;
	int removed_last_with_datap = 0;

	EVBUFFER_LOCK(buf);

	chain = buf->first;

	if (size < 0)
		size = buf->total_len;
	/* if size > buf->total_len, we cannot guarantee to the user that she
	 * is going to have a long enough buffer afterwards; so we return
	 * NULL */
	if (size == 0 || (size_t)size > buf->total_len)
		goto done;

	/* No need to pull up anything; the first size bytes are
	 * already here. */
	if (chain->off >= (size_t)size) {
		result = chain->buffer + chain->misalign;
		goto done;
	}

	/* Make sure that none of the chains we need to copy from is pinned. */
	remaining = size - chain->off;
	EVUTIL_ASSERT(remaining >= 0);
	for (tmp=chain->next; tmp; tmp=tmp->next) {
		if (CHAIN_PINNED(tmp))
			goto done;
		if (tmp->off >= (size_t)remaining)
			break;
		remaining -= tmp->off;
	}

	if (CHAIN_PINNED(chain)) {
		size_t old_off = chain->off;
		if (CHAIN_SPACE_LEN(chain) < size - chain->off) {
			/* not enough room at end of chunk. */
			goto done;
		}
		buffer = CHAIN_SPACE_PTR(chain);
		tmp = chain;
		tmp->off = size;
		size -= old_off;
		chain = chain->next;
	} else if (chain->buffer_len - chain->misalign >= (size_t)size) {
		/* already have enough space in the first chain */
		size_t old_off = chain->off;
		buffer = chain->buffer + chain->misalign + chain->off;
		tmp = chain;
		tmp->off = size;
		size -= old_off;
		chain = chain->next;
	} else {
		if ((tmp = evbuffer_chain_new(size)) == NULL) {
			event_warn("%s: out of memory", __func__);
			goto done;
		}
		buffer = tmp->buffer;
		tmp->off = size;
		buf->first = tmp;
	}

	/* TODO(niels): deal with buffers that point to NULL like sendfile */

	/* Copy and free every chunk that will be entirely pulled into tmp */
	last_with_data = *buf->last_with_datap;
	for (; chain != NULL && (size_t)size >= chain->off; chain = next) {
		next = chain->next;

		memcpy(buffer, chain->buffer + chain->misalign, chain->off);
		size -= chain->off;
		buffer += chain->off;
		if (chain == last_with_data)
			removed_last_with_data = 1;
		if (&chain->next == buf->last_with_datap)
			removed_last_with_datap = 1;

		evbuffer_chain_free(chain);
	}

	if (chain != NULL) {
		memcpy(buffer, chain->buffer + chain->misalign, size);
		chain->misalign += size;
		chain->off -= size;
	} else {
		buf->last = tmp;
	}

	tmp->next = chain;

	if (removed_last_with_data) {
		buf->last_with_datap = &buf->first;
	} else if (removed_last_with_datap) {
		if (buf->first->next && buf->first->next->off)
			buf->last_with_datap = &buf->first->next;
		else
			buf->last_with_datap = &buf->first;
	}

	result = (tmp->buffer + tmp->misalign);

done:
	EVBUFFER_UNLOCK(buf);
	return result;
}//end evbuffer_pullup



#define EVBUFFER_CHAIN_MAX_AUTO_SIZE 4096

//Buffer的数据操作：
//
//在链表尾添加数据：
//Libevent提供给用户的添加数据接口是evbuffer_add，现在就通过这个函数看一下是怎么将数据插入到buffer中的。
//该函数是在链表的尾部添加数据，如果想在链表的前面添加数据可以使用evbuffer_prepend。
//
//在链表尾部插入数据，分下面几种情况：
//1.该链表为空，即这是第一次插入数据。这是最简单的，直接把新建的evbuffer_chain插入到链表中，通过调用evbuffer_chain_insert。
//2.链表的最后一个节点(即evbuffer_chain)还有一些空余的空间，放得下本次要插入的数据。此时直接把数据追加到最后一个节点即可。
//3.链表的最后一个节点并不能放得下本次要插入的数据，那么就需要把本次要插入的数据分开由两个evbuffer_chain存放。

//evbuffer_add函数是复制一份数据，保存在链表中。这样做的好处是，用户调用该函数后，就可以丢弃该数据。
//bufferevent_write是直接调用这个函数。
//当用户调用bufferevent_write后，就可以马上把数据丢弃，无需等到Libevent把这份数据写到socket的缓存区中。
int
evbuffer_add(struct evbuffer *buf, const void *data_in, size_t datlen)
{
	struct evbuffer_chain *chain, *tmp;
	const unsigned char *data = data_in;
	size_t remain, to_alloc;
	int result = -1;

	EVBUFFER_LOCK(buf);

	if (buf->freeze_end) {
		goto done;
	}

	//找到最后一个evbuffer_chain。
	chain = buf->last;

	//第一次插入数据时，buf->last为NULL 
	if (chain == NULL) {
		chain = evbuffer_chain_new(datlen);
		if (!chain)
			goto done;
		evbuffer_chain_insert(buf, chain);
	}

	//EVBUFFER_IMMUTABLE 是 read-only chain 
	if ((chain->flags & EVBUFFER_IMMUTABLE) == 0) {//等于0说明是可以写的
		//最后那个chain可以放的字节数
		remain = (size_t)(chain->buffer_len - chain->misalign - chain->off);
		//最后那个chain可以放下本次要插入的数据
		if (remain >= datlen) {
			memcpy(chain->buffer + chain->misalign + chain->off,
			    data, datlen);
			chain->off += datlen;
			buf->total_len += datlen;//偏移量，方便下次插入数据
			buf->n_add_for_cb += datlen;//buffer的总字节数
			goto out;
		} else if (!CHAIN_PINNED(chain) && //该evbuffer_chain可以修改
		    evbuffer_chain_should_realign(chain, datlen)) {//通过调整后，也可以放得下本次要插入的数据
			//通过使用chain->misalign这个错位空间而插入数据
			evbuffer_chain_align(chain);

			memcpy(chain->buffer + chain->off, data, datlen);
			chain->off += datlen;
			buf->total_len += datlen;
			buf->n_add_for_cb += datlen;
			goto out;
		}
	} else {
		/* we cannot write any data to the last chain */
		remain = 0;
	}
	
    //当这个evbuffer_chain是一个read-only buffer或者最后那个chain  
    //放不下本次要插入的数据时才会执行下面代码  
    //此时需要新建一个evbuffer_chain 
	to_alloc = chain->buffer_len;
    //当最后evbuffer_chain的缓冲区小于等于2048时，那么新建的evbuffer_chain的  
    //大小将是最后一个节点缓冲区的2倍。 
	if (to_alloc <= EVBUFFER_CHAIN_MAX_AUTO_SIZE/2)
		to_alloc <<= 1;
    //最后的大小还是有要插入的数据决定。要注意的是虽然to_alloc最后的值可能为  
    //datlen。但在evbuffer_chain_new中，实际分配的内存大小必然是512的倍数。
	if (datlen > to_alloc)
		to_alloc = datlen;

    //此时需要new一个chain才能保存本次要插入的数据  
	tmp = evbuffer_chain_new(to_alloc);
	if (tmp == NULL)
		goto done;

	//链表最后那个节点还是可以放下一些数据的。那么就先填满链表最后那个节点
	if (remain) {
		memcpy(chain->buffer + chain->misalign + chain->off,
		    data, remain);
		chain->off += remain;
		buf->total_len += remain;
		buf->n_add_for_cb += remain;
	}

	data += remain;//要插入的数据指针
	datlen -= remain;

	//把要插入的数据复制到新建一个chain中。
	memcpy(tmp->buffer, data, datlen);
	tmp->off = datlen;
	//将这个chain插入到evbuffer中 
	evbuffer_chain_insert(buf, tmp);
	buf->n_add_for_cb += datlen;

out:
	evbuffer_invoke_callbacks(buf);
	result = 0;
done:
	EVBUFFER_UNLOCK(buf);
	return result;
}

static void
evbuffer_chain_align(struct evbuffer_chain *chain)
{
	EVUTIL_ASSERT(!(chain->flags & EVBUFFER_IMMUTABLE));
	EVUTIL_ASSERT(!(chain->flags & EVBUFFER_MEM_PINNED_ANY));
	memmove(chain->buffer, chain->buffer + chain->misalign, chain->off);
	chain->misalign = 0;
}

#define MAX_TO_COPY_IN_EXPAND 4096
#define MAX_TO_REALIGN_IN_EXPAND 2048

static int
evbuffer_chain_should_realign(struct evbuffer_chain *chain,
    size_t datlen)
{
	return chain->buffer_len - chain->off >= datlen &&
	    (chain->off < chain->buffer_len / 2) &&
	    (chain->off <= MAX_TO_REALIGN_IN_EXPAND);
}

//evbuffer_expand_singlechain函数是要求一个节点就能提供大小为datlen的可用空间。
static struct evbuffer_chain *
evbuffer_expand_singlechain(struct evbuffer *buf, size_t datlen)
{
	struct evbuffer_chain *chain, **chainp;
	struct evbuffer_chain *result = NULL;
	ASSERT_EVBUFFER_LOCKED(buf);

	chainp = buf->last_with_datap;

    //*chainp指向最后一个有数据的evbuffer_chain或者为NULL 
	//CHAIN_SPACE_LEN该chain可用空间的大小
	if (*chainp && CHAIN_SPACE_LEN(*chainp) == 0)
		chainp = &(*chainp)->next;

	//经过上面的那个if后，当最后一个有数据的evbuffer_chain还有空闲空间时  
    //*chainp就指向之。否则*chainp指向最后一个有数据的evbuffer_chain的next。 
	chain = *chainp;

	//这个chain是不可修改的，那么就只能插入一个新的chain了
	if (chain == NULL ||
	    (chain->flags & (EVBUFFER_IMMUTABLE|EVBUFFER_MEM_PINNED_ANY))) {
		goto insert_new;
	}

	//这个chain的可用空间大于扩展空间
	//这种情况，Libevent并不会扩大buffer空间.
	//因为Libevent认为现在的可用空间可以用作用户提出的预留空间  
	if (CHAIN_SPACE_LEN(chain) >= datlen) {
		result = chain;
		goto ok;
	}

	/* If the chain is completely empty, just replace it by adding a new
	 * empty chain. */
	if (chain->off == 0) {
		goto insert_new;
	}

	/* If the misalignment plus the remaining space fulfills our data
	 * needs, we could just force an alignment to happen.  Afterwards, we
	 * have enough space.  But only do this if we're saving a lot of space
	 * and not moving too much data.  Otherwise the space savings are
	 * probably offset by the time lost in copying.
	 */
     //通过使用misalign错位空间，也能使得可用空间大于等于预留空间，那么也不用  
     //扩大buffer空间  
	if (evbuffer_chain_should_realign(chain, datlen)) {
		evbuffer_chain_align(chain);
		result = chain;
		goto ok;
	}

	/* At this point, we can either resize the last chunk with space in
	 * it, use the next chunk after it, or   If we add a new chunk, we waste
	 * CHAIN_SPACE_LEN(chain) bytes in the former last chunk.  If we
	 * resize, we have to copy chain->off bytes.
	 */

	/* Would expanding this chunk be affordable and worthwhile? */
    //空闲空间小于总空间的1/8
	//或者 已有的数据量大于MAX_TO_COPY_IN_EXPAND(4096)   
	if (CHAIN_SPACE_LEN(chain) < chain->buffer_len / 8 ||
	    chain->off > MAX_TO_COPY_IN_EXPAND) {
		//本chain有比较多的数据，将这些数据迁移到另外一个chain是不划算的  
        //此时，将不会改变这个chain。  
        //下一个chain是否可以有足够的空闲空间.有则直接用之  
		/* It's not worth resizing this chain. Can the next one be
		 * used? */
		if (chain->next && CHAIN_SPACE_LEN(chain->next) >= datlen) {
			/* Yes, we can just use the next chain (which should
			 * be empty. */
			result = chain->next;
			goto ok;
		} else {
			/* No; append a new chain (which will free all
			 * terminal empty chains.) */
			goto insert_new;
		}
	} else {
        //由于本chain的数据量比较小，所以把这个chain的数据迁移到另外一个  
        //chain上是值得的。
		/* Okay, we're going to try to resize this chain: Not doing so
		 * would waste at least 1/8 of its current allocation, and we
		 * can do so without having to copy more than
		 * MAX_TO_COPY_IN_EXPAND bytes. */
		/* figure out how much space we need */
		size_t length = chain->off + datlen;
		struct evbuffer_chain *tmp = evbuffer_chain_new(length);
		if (tmp == NULL)
			goto err;

		/* copy the data over that we had so far */
		tmp->off = chain->off;
		//进行数据迁移
		memcpy(tmp->buffer, chain->buffer + chain->misalign,
		    chain->off);
		/* fix up the list */
		EVUTIL_ASSERT(*chainp == chain);
		result = *chainp = tmp;

		if (buf->last == chain)
			buf->last = tmp;

		tmp->next = chain->next;
		evbuffer_chain_free(chain);
		goto ok;
	}

insert_new:
	result = evbuffer_chain_insert_new(buf, datlen);
	if (!result)
		goto err;
ok:
	EVUTIL_ASSERT(result);
	EVUTIL_ASSERT(CHAIN_SPACE_LEN(result) >= datlen);
err:
	return result;
}//end evbuffer_expand_singlechain

//_evbuffer_expand_fast函数，该函数还有一个整型的参数n，
//用来表示使用不超过n个节点的前提下，提供datlen的可用空间。
//不过这个函数只留给Libevent内部使用，用户不能使用之。
//用最多不超过n个节点就提供datlen大小的空闲空间。链表过长是不好的
int
_evbuffer_expand_fast(struct evbuffer *buf, size_t datlen, int n)
{
	struct evbuffer_chain *chain = buf->last, *tmp, *next;
	size_t avail;
	int used;

	ASSERT_EVBUFFER_LOCKED(buf);
	//n必须大于等于2
	EVUTIL_ASSERT(n >= 2);

    //最后一个节点是不可用的  
	if (chain == NULL || (chain->flags & EVBUFFER_IMMUTABLE)) {
		//这种情况下，直接新建一个足够大的evbuffer_chain即可 
		/* There is no last chunk, or we can't touch the last chunk.
		 * Just add a new chunk. */
		chain = evbuffer_chain_new(datlen);
		if (chain == NULL)
			return (-1);

		evbuffer_chain_insert(buf, chain);
		return (0);
	}

	used = 0; /* number of chains we're using space in. */
	avail = 0; /* how much space they have. */
	/* How many bytes can we stick at the end of buffer as it is?  Iterate
	 * over the chains at the end of the buffer, tring to see how much
	 * space we have in the first n. */
	for (chain = *buf->last_with_datap; chain; chain = chain->next) {
		if (chain->off) {//最后一个有数据的节点的可用空间也是要被使用
			size_t space = (size_t) CHAIN_SPACE_LEN(chain);
			EVUTIL_ASSERT(chain == *buf->last_with_datap);
			if (space) {
				avail += space;
				++used;
			}
		} else {//链表中off为0的空buffer统统使用 
			/* No data in chain; realign it. */
			chain->misalign = 0;
			avail += chain->buffer_len;
			++used;
		}
		if (avail >= datlen) {
			//链表中的节点的可用空间已经足够了
			/* There is already enough space.  Just return */
			return (0);
		}
		//到达了最大可以忍受的链表长度  
		if (used == n)
			break;
	}

    //前面的for循环，如果找够了空闲空间，那么是直接return。所以  
    //运行到这里时，就说明还没找到空闲空间。一般是因为链表后面的off等于0  
    //的节点已经被用完了都还不能满足datlen 
	/* There wasn't enough space in the first n chains with space in
	 * them. Either add a new chain with enough space, or replace all
	 * empty chains with one that has enough space, depending on n. */
	if (used < n) {
		/* The loop ran off the end of the chains before it hit n
		 * chains; we can add another. */
		EVUTIL_ASSERT(chain == NULL);

        //申请一个足够大的evbuffer_chain，把空间补足  
		tmp = evbuffer_chain_new(datlen - avail);
		if (tmp == NULL)
			return (-1);

		buf->last->next = tmp;
		buf->last = tmp;
		/* (we would only set last_with_data if we added the first
		 * chain. But if the buffer had no chains, we would have
		 * just allocated a new chain earlier) */
		return (0);
	} else {
		//used == n。
		//把后面的n个节点都用了还是不够datlen空间  链表后面的n个节点都用上了，
		//这个n个节点中，至少有n-1个节点的off等于0。
		//n个节点都不够，
		//Libevent就认为这些节点都是饭桶，Libevent会统统删除  
        //然后新建一个足够大的evbuffer_chain。
        //用来标志该链表的所有节点都是off为0的。
		//在这种情况下，将删除所有的节点  

		/* Nuke _all_ the empty chains. */
		int rmv_all = 0; /* True iff we removed last_with_data. */
		chain = *buf->last_with_datap;
		if (!chain->off) {
			//这说明链表中的节点都是没有数据的evbuffer_chain
			EVUTIL_ASSERT(chain == buf->first);
			rmv_all = 1;
			avail = 0;
		} else {
			//最后一个有数据的chain的可用空间的大小。这个空间是可以用上的 
			avail = (size_t) CHAIN_SPACE_LEN(chain);
			chain = chain->next;
		}
        //将这些off等于0的evbuffer_chain统统free掉，不要了。  
        //然后new一个足够大的evbuffer_chain即可。这能降低链表的长度 
		for (; chain; chain = next) {
			next = chain->next;
			EVUTIL_ASSERT(chain->off == 0);
			evbuffer_chain_free(chain);
		}

        //new一个足够大的evbuffer_chain  
        tmp = evbuffer_chain_new(datlen - avail);  
        if (tmp == NULL) {//new失败  
            if (rmv_all) {//这种情况下，该链表就根本没有节点了  
                ZERO_CHAIN(buf);//相当于初始化evbuffer的链表  
            } else {  
                buf->last = *buf->last_with_datap;  
                (*buf->last_with_datap)->next = NULL;  
            }  
            return (-1);  
        }  

		if (rmv_all) {//这种情况下，该链表就只有一个节点了
			buf->first = buf->last = tmp;
			buf->last_with_datap = &buf->first;
		} else {
			(*buf->last_with_datap)->next = tmp;
			buf->last = tmp;
		}
		return (0);
	}
}//end _evbuffer_expand_fast

int
evbuffer_expand(struct evbuffer *buf, size_t datlen)
{
	struct evbuffer_chain *chain;

	EVBUFFER_LOCK(buf);
	chain = evbuffer_expand_singlechain(buf, datlen);
	EVBUFFER_UNLOCK(buf);
	return chain ? 0 : -1;
}

#if defined(_EVENT_HAVE_SYS_UIO_H) || defined(WIN32)
#define USE_IOVEC_IMPL
#endif

#ifdef USE_IOVEC_IMPL

#ifdef _EVENT_HAVE_SYS_UIO_H
#define DEFAULT_WRITE_IOVEC 128

#if defined(UIO_MAXIOV) && UIO_MAXIOV < DEFAULT_WRITE_IOVEC
#define NUM_WRITE_IOVEC UIO_MAXIOV
#elif defined(IOV_MAX) && IOV_MAX < DEFAULT_WRITE_IOVEC
#define NUM_WRITE_IOVEC IOV_MAX
#else
#define NUM_WRITE_IOVEC DEFAULT_WRITE_IOVEC
#endif

#define IOV_TYPE struct iovec
#define IOV_PTR_FIELD iov_base
#define IOV_LEN_FIELD iov_len
#define IOV_LEN_TYPE size_t
#else
#define NUM_WRITE_IOVEC 16
#define IOV_TYPE WSABUF
#define IOV_PTR_FIELD buf
#define IOV_LEN_FIELD len
#define IOV_LEN_TYPE unsigned long
#endif
#endif

#define NUM_READ_IOVEC 4

#define EVBUFFER_MAX_READ	4096

//让vecs数组的指针指向evbuffer中的可用chain.标明哪个chain可用并且从chain的哪里开始，以及可用的字节数  
//howmuch是要扩容的大小。vecs、n_vecs_avail分别是iovec数组和数组的大小  
//chainp是值-结果参数，它最后指向第一个有可用空间的chain  
int
_evbuffer_read_setup_vecs(struct evbuffer *buf, ev_ssize_t howmuch,
    struct evbuffer_iovec *vecs, int n_vecs_avail,
    struct evbuffer_chain ***chainp, int exact)
{
	struct evbuffer_chain *chain;
	struct evbuffer_chain **firstchainp;
	size_t so_far;
	int i;
	ASSERT_EVBUFFER_LOCKED(buf);

	if (howmuch < 0)
		return -1;

	so_far = 0;
	/* Let firstchain be the first chain with any space on it */
    //因为找的是evbuffer链表中的空闲空间，所以从最后一个有数据的chain中开始找  
	firstchainp = buf->last_with_datap;
	//这个chain已经没有空间了
	if (CHAIN_SPACE_LEN(*firstchainp) == 0) {
		//那么只能下一个chain了
		firstchainp = &(*firstchainp)->next;
	}

    //因为Libevent在调用本函数之前，一般会调用_evbuffer_expand_fast来扩大  
    //evbuffer的可用空间。所以下面的循环中并没有判断chain是否为NULL，就直接  
    //chain->next  
	chain = *firstchainp;
	for (i = 0; i < n_vecs_avail && so_far < (size_t)howmuch; ++i) {
		size_t avail = (size_t) CHAIN_SPACE_LEN(chain);
        //如果exact为真，那么即使这个chain有更多的可用空间，也不会使用。只会  
        //要自己正需要的空间  
		if (avail > (howmuch - so_far) && exact)
			avail = howmuch - so_far;
		vecs[i].iov_base = CHAIN_SPACE_PTR(chain);///这个chain的可用空间的开始位置
		vecs[i].iov_len = avail;//可用长度
		so_far += avail;
		chain = chain->next;
	}

	*chainp = firstchainp;//指向第一个有可用空间的chain
	return i;//返回需要多少个chain才能有howmuch这么多的空闲空间
}//end _evbuffer_read_setup_vecs

static int
get_n_bytes_readable_on_socket(evutil_socket_t fd)
{
#if defined(FIONREAD) && defined(WIN32)
	unsigned long lng = EVBUFFER_MAX_READ;
	if (ioctlsocket(fd, FIONREAD, &lng) < 0)
		return -1;
	return (int)lng;
#elif defined(FIONREAD)
	int n = EVBUFFER_MAX_READ;
	if (ioctl(fd, FIONREAD, &n) < 0)
		return -1;
	return n;
#else
	return EVBUFFER_MAX_READ;
#endif
}// end get_n_bytes_readable_on_socket

//由于存在是否支持类似readv函数 这两种情况，所以evbuffer_read在实现上也出现了两种实现。
//返回读取到的字节数。错误返回-1，断开了连接返回0  
//howmuch指出此时evbuffer可以使用的空间大小
int
evbuffer_read(struct evbuffer *buf, evutil_socket_t fd, int howmuch)
{
	struct evbuffer_chain **chainp;
	int n;
	int result;

//所在的系统支持iovec或者是Windows操作系统
#ifdef USE_IOVEC_IMPL
	int nvecs, i, remaining;
#else
	struct evbuffer_chain *chain;
	unsigned char *p;
#endif

	EVBUFFER_LOCK(buf);

	if (buf->freeze_end) {
		result = -1;
		goto done;
	}

	//获取这个socket接收缓冲区里面有多少字节可读.通过ioctl实现  
	n = get_n_bytes_readable_on_socket(fd);
	//每次只读EVBUFFER_MAX_READ(4096)个字符
	if (n <= 0 || n > EVBUFFER_MAX_READ)
		n = EVBUFFER_MAX_READ;
	if (howmuch < 0 || howmuch > n)
		howmuch = n;

//所在的系统支持iovec或者是Windows操作系统
#ifdef USE_IOVEC_IMPL
    //NUM_READ_IOVEC等于4  
    //扩大evbuffer,使得其有howmuch字节的空闲空间  
    //在NUM_READ_IOVEC个evbuffer_chain中扩容足够的空闲空间 
	if (_evbuffer_expand_fast(buf, howmuch, NUM_READ_IOVEC) == -1) {
		result = -1;
		goto done;
	} else {
        //在posix中IOV_TYPE为iovec，在Windows中为WSABUF  
		IOV_TYPE vecs[NUM_READ_IOVEC];
#ifdef _EVBUFFER_IOVEC_IS_NATIVE //所在的系统支持iovec结构体 
		nvecs = _evbuffer_read_setup_vecs(buf, howmuch, vecs,
		    NUM_READ_IOVEC, &chainp, 1);
#else//Windows系统。因为没有native的 iovec
		/* We aren't using the native struct iovec.  Therefore,
		   we are on win32. */
#endif
		n = readv(fd, vecs, nvecs);
	}

#else /*!USE_IOVEC_IMPL*/
	//如果所在的系统不支持 iovec并且不是Windows系统。也就是说不支持类似  
	//readv这样的函数。那么只能把所有的数据都读到一个chain中  
	//把一个chain扩大得可以有howmuch字节的空闲空间
	/* If we don't have FIONREAD, we might waste some space here */
	/* XXX we _will_ waste some space here if there is any space left
	 * over on buf->last. */
	if ((chain = evbuffer_expand_singlechain(buf, howmuch)) == NULL) {
		result = -1;
		goto done;
	}

	/* We can append new data at this point */
	p = chain->buffer + chain->misalign + chain->off;

	n = read(fd, p, howmuch);
#endif /* USE_IOVEC_IMPL */

	if (n == -1) {
		result = -1;
		goto done;
	}
	if (n == 0) {
		result = 0;
		goto done;
	}

#ifdef USE_IOVEC_IMPL
	//使用了iovec结构体读取数据。需要做一些额外的处理  
    //chainp是由_evbuffer_read_setup_vecs函数调用得到。它指向未从fd读取数据时  
    //第一个有空闲空间位置的chain  
	remaining = n;//n等于读取到的字节数 
	//使用iovec读取数据时，只是把数据往chain中填充，并没有修改evbuffer_chain  
    //的成员，比如off偏移量成员。此时就需要把这个off修改到正确值
	for (i=0; i < nvecs; ++i) {
        //CHAIN_SPACE_LEN(*chainp)返回的是填充数据前的空闲空间。  
        //除了最后那个chain外，其他的chain都会被填满的。所以对于非last  
        //chain，直接把off加上这个space即可。
		ev_ssize_t space = (ev_ssize_t) CHAIN_SPACE_LEN(*chainp);
		if (space < remaining) {//前面的chain
			(*chainp)->off += space;
			remaining -= (int)space;
		} else {//最后那个chain
			(*chainp)->off += remaining;
			buf->last_with_datap = chainp;
			break;
		}
		chainp = &(*chainp)->next;
	}
#else
	chain->off += n;
    //调整last_with_datap，使得*last_with_datap指向最后一个有数据的chain  
	advance_last_with_data(buf);
#endif
	buf->total_len += n;
	buf->n_add_for_cb += n;//添加了n字节

	/* Tell someone about changes in this buffer */
	evbuffer_invoke_callbacks(buf);//evbuffer添加了数据，就需要调用回调函数
	result = n;
done:
	EVBUFFER_UNLOCK(buf);
	return result;
}//end evbuffer_read

#ifdef USE_IOVEC_IMPL
static inline int
evbuffer_write_iovec(struct evbuffer *buffer, evutil_socket_t fd,
    ev_ssize_t howmuch)
{
	IOV_TYPE iov[NUM_WRITE_IOVEC];
	struct evbuffer_chain *chain = buffer->first;
	int n, i = 0;

	if (howmuch < 0)
		return -1;

	ASSERT_EVBUFFER_LOCKED(buffer);
	/* XXX make this top out at some maximal data length?  if the
	 * buffer has (say) 1MB in it, split over 128 chains, there's
	 * no way it all gets written in one go. */
	while (chain != NULL && i < NUM_WRITE_IOVEC && howmuch) {
#ifdef USE_SENDFILE
		/* we cannot write the file info via writev */
		if (chain->flags & EVBUFFER_SENDFILE)
			break;
#endif
		iov[i].IOV_PTR_FIELD = (void *) (chain->buffer + chain->misalign);
		if ((size_t)howmuch >= chain->off) {
			/* XXXcould be problematic when windows supports mmap*/
			iov[i++].IOV_LEN_FIELD = (IOV_LEN_TYPE)chain->off;
			howmuch -= chain->off;
		} else {
			/* XXXcould be problematic when windows supports mmap*/
			iov[i++].IOV_LEN_FIELD = (IOV_LEN_TYPE)howmuch;
			break;
		}
		chain = chain->next;
	}
	if (! i)
		return 0;
	n = writev(fd, iov, i);
	return (n);
}//end evbuffer_write_iovec
#endif

int//howmuch是要写的字节数。如果小于0，那么就把buffer里的所有数据都写入fd  
evbuffer_write_atmost(struct evbuffer *buffer, evutil_socket_t fd,  
    ev_ssize_t howmuch)  
{  
    int n = -1;  
  
    EVBUFFER_LOCK(buffer);  
  
    //冻结了链表头，无法往fd写数据。因为写之后，还要把数据从evbuffer中删除  
    if (buffer->freeze_start) {  
        goto done;  
    }  
  
    if (howmuch < 0 || (size_t)howmuch > buffer->total_len)  
        howmuch = buffer->total_len;  
  
    if (howmuch > 0) {  
#ifdef USE_SENDFILE //所在的系统支持sendfile  
        struct evbuffer_chain *chain = buffer->first;  
        //需通过evbuffer_add_file添加数据，才会使用sendfile  
        if (chain != NULL && (chain->flags & EVBUFFER_SENDFILE)) //并且要求使用sendfile  
            n = evbuffer_write_sendfile(buffer, fd, howmuch);  
        else {  
#endif  
#ifdef USE_IOVEC_IMPL //所在的系统支持writev这类函数  
        //函数内部会设置数组元素的成员指针，以及长度成员  
        n = evbuffer_write_iovec(buffer, fd, howmuch);  
#elif defined(WIN32)  
        /* XXX(nickm) Don't disable this code until we know if 
         * the WSARecv code above works. */  
        //把evbuffer前面的howmuch字节拉直。使得这howmuch字节都放在一个chain里面  
        //也就是放在一个连续的空间，不再是之前的多个链表节点。这样就能直接用  
        //send函数发送了。  
        void *p = evbuffer_pullup(buffer, howmuch);  
        n = send(fd, p, howmuch, 0);  
#else  
        void *p = evbuffer_pullup(buffer, howmuch);  
        n = write(fd, p, howmuch);  
#endif  
#ifdef USE_SENDFILE  
        }  
#endif  
    }  
  
    if (n > 0)  
        evbuffer_drain(buffer, n);//从链表中删除已经写入到socket的n个字节  
  
done:  
    EVBUFFER_UNLOCK(buffer);  
    return (n);  
}  




//往socket写入数据：

//因为evbuffer是用链表的形式存放数据，所以要把这些链表上的数据写入socket，
//那么使用writev这个函数是十分有效的。同前面一样，使用iovec结构体数组，就需要设置数组元素的指针。
//这个工作由evbuffer_write_iovec函数完成。
 
//正如前面的从socket读出数据，可能所在的系统并不支持writev这样的函数。
//此时就只能使用一般的write函数了，但这个函数要求数据放在一个连续的空间。
//所以Libevent有一个函数evbuffer_pullup，用来把链表内存拉直，即把一定数量的数据从链表中copy到一个连续的内存空间。
//这个连续的空间也是由某个evbuffer_chain的buffer指针指向，并且这个evbuffer_chain会被插入到链表中。
//这个时候就可以直接使用write或者send函数发送这特定数量的数据了。
 
//不同于读，写操作还有第三种可能。那就是sendfile。
//如果所在的系统支持sendfile，并且用户是通过evbuffer_add_file添加数据的，
//那么此时Libevent就是所在系统的sendfile函数发送数据。
 
//Libevent内部一般通过evbuffer_write函数把数据写入到socket fd中。下面是具体的实现。
int
evbuffer_write(struct evbuffer *buffer, evutil_socket_t fd)
{
    //把evbuffer的所有数据都写入到fd中  
	return evbuffer_write_atmost(buffer, fd, -1);
}

//设置evbuffer_ptr。evbuffer_ptr_set(buf, &pos, 0, EVBUFFER_PTR_SET)  
//将这个pos指向链表的开头  
//position指明移动的偏移量，how指明该偏移量是绝对偏移量还是相对当前位置的偏移量。  
//这个函数的作用就像C语言中的fseek，设置文件指针的偏移量

//可以看到，该函数只考虑了向后面的chain移动定位指针，不能向。
//当然如果参数position小于0，并且移动时并不会跨越当前的chain，还是可以的。不过最好不要这样做。
//如果确实想移回头，那么可以考虑下面的操作。
//pos.position -= 20;//移回头20个字节。  
//evbuffer_ptr_set(buf, &pos, 0, EVBUFFER_PTR_SET);  
int
evbuffer_ptr_set(struct evbuffer *buf, struct evbuffer_ptr *pos,
    size_t position, enum evbuffer_ptr_how how)
{
	size_t left = position;
	struct evbuffer_chain *chain = NULL;

	EVBUFFER_LOCK(buf);

    //这个switch的作用就是给pos设置新的总偏移量值。  
	switch (how) {
    case EVBUFFER_PTR_SET://绝对位置  
        chain = buf->first;//从第一个evbuffer_chain算起  
        pos->pos = position; //设置总偏移量  
        position = 0;  
        break;  
    case EVBUFFER_PTR_ADD://相对位置  
        chain = pos->_internal.chain;//从当前evbuffer_chain算起  
        pos->pos += position;//加上相对偏移量  
        position = pos->_internal.pos_in_chain;  
        break;
	}

	//这个偏移量跨了evbuffer_chain。可能不止跨一个chain。 
	while (chain && position + left >= chain->off) {
		left -= chain->off - position;
		chain = chain->next;
		position = 0;
	}
	if (chain) {
		pos->_internal.chain = chain;
		pos->_internal.pos_in_chain = position + left;
	} else {
		pos->_internal.chain = NULL;
		pos->pos = -1;
	}

	EVBUFFER_UNLOCK(buf);

	return chain != NULL ? 0 : -1;
}//end evbuffer_ptr_set


//匹配成功会返回0
static int
evbuffer_ptr_memcmp(const struct evbuffer *buf, const struct evbuffer_ptr *pos,
    const char *mem, size_t len)
{
	struct evbuffer_chain *chain;
	size_t position;
	int r;

	ASSERT_EVBUFFER_LOCKED(buf);

    //链表数据不够  
	if (pos->pos + len > buf->total_len)
		return -1;

	//需要考虑这个要匹配的字符串被分散在两个evbuffer_chain中
	chain = pos->_internal.chain;
	position = pos->_internal.pos_in_chain;//从evbuffer_chain中的这个位置开始
	while (len && chain) {
		size_t n_comparable;
		if (len + position > chain->off)
			n_comparable = chain->off - position;
		else
			n_comparable = len;
		r = memcmp(chain->buffer + chain->misalign + position, mem,
		    n_comparable);
		if (r)
			return r;
		mem += n_comparable;
		len -= n_comparable;
		position = 0;
		chain = chain->next;
	}

	return 0;
}//end evbuffer_ptr_memcmp

//无论是删除数据还是添加数据的函数，例如evbuffer_add和evbuffer_drain函数，
//都是会调用evbuffer_invoke_callbacks函数的。而这个函数会调用evbuffer_run_callbacks函数。
static void
evbuffer_run_callbacks(struct evbuffer *buffer, int running_deferred)
{
	struct evbuffer_cb_entry *cbent, *next;
	struct evbuffer_cb_info info;
	size_t new_size;
	ev_uint32_t mask, masked_val;
	int clear = 1;

	if (running_deferred) {
		mask = EVBUFFER_CB_NODEFER|EVBUFFER_CB_ENABLED;
		masked_val = EVBUFFER_CB_ENABLED;
	} else if (buffer->deferred_cbs) {
		mask = EVBUFFER_CB_NODEFER|EVBUFFER_CB_ENABLED;
		masked_val = EVBUFFER_CB_NODEFER|EVBUFFER_CB_ENABLED;
		/* Don't zero-out n_add/n_del, since the deferred callbacks
		   will want to see them. */
		clear = 0;
	} else {//一般都是这种情况
		mask = EVBUFFER_CB_ENABLED;
		masked_val = EVBUFFER_CB_ENABLED;
	}

	ASSERT_EVBUFFER_LOCKED(buffer);

	if (TAILQ_EMPTY(&buffer->callbacks)) {//用户没有设置回调函数
		//清零
		buffer->n_add_for_cb = buffer->n_del_for_cb = 0;
		return;
	}
	//没有添加或者删除数据
	if (buffer->n_add_for_cb == 0 && buffer->n_del_for_cb == 0)
		return;

	new_size = buffer->total_len;
	info.orig_size = new_size + buffer->n_del_for_cb - buffer->n_add_for_cb;
	info.n_added = buffer->n_add_for_cb;
	info.n_deleted = buffer->n_del_for_cb;
	if (clear) {//清零，为下次计算做准备
		buffer->n_add_for_cb = 0;
		buffer->n_del_for_cb = 0;
	}

    //遍历回调函数队列，调用回调函数  
	for (cbent = TAILQ_FIRST(&buffer->callbacks);
	     cbent != TAILQ_END(&buffer->callbacks);
	     cbent = next) {
		/* Get the 'next' pointer now in case this callback decides
		 * to remove itself or something. */
		next = TAILQ_NEXT(cbent, next);

        //该回调函数没有enable  
		if ((cbent->flags & mask) != masked_val)
			continue;

		if ((cbent->flags & EVBUFFER_CB_OBSOLETE))//已经不被推荐使用了
			cbent->cb.cb_obsolete(buffer,
			    info.orig_size, new_size, cbent->cbarg);
		else
			cbent->cb.cb_func(buffer, &info, cbent->cbarg);//调用用户设置的回调函数
	}
}//end evbuffer_run_callbacks


void
evbuffer_invoke_callbacks(struct evbuffer *buffer)
{
	if (TAILQ_EMPTY(&buffer->callbacks)) {
		buffer->n_add_for_cb = buffer->n_del_for_cb = 0;
		return;
	}

	if (buffer->deferred_cbs) {
		if (buffer->deferred.queued)
			return;
		_evbuffer_incref_and_lock(buffer);
		if (buffer->parent)
			bufferevent_incref(buffer->parent);
		EVBUFFER_UNLOCK(buffer);
		event_deferred_cb_schedule(buffer->cb_queue, &buffer->deferred);
	}

	evbuffer_run_callbacks(buffer, 0);
}

void
evbuffer_setcb(struct evbuffer *buffer, evbuffer_cb cb, void *cbarg)
{
	EVBUFFER_LOCK(buffer);

	if (!TAILQ_EMPTY(&buffer->callbacks))
		evbuffer_remove_all_callbacks(buffer);

	if (cb) {
		struct evbuffer_cb_entry *ent =
		    evbuffer_add_cb(buffer, NULL, cbarg);
		ent->cb.cb_obsolete = cb;
		ent->flags |= EVBUFFER_CB_OBSOLETE;
	}
	EVBUFFER_UNLOCK(buffer);
}

//参数cbarg就是回调函数被调用时的那个arg参数，这点对于熟悉Libevent的读者应该不难理解。
//上面这个函数是被一个evbuffer_cb_entry结构体指针插入到callbacks队列的前面
struct evbuffer_cb_entry *
evbuffer_add_cb(struct evbuffer *buffer, evbuffer_cb_func cb, void *cbarg)
{
	struct evbuffer_cb_entry *e;
	if (! (e = mm_calloc(1, sizeof(struct evbuffer_cb_entry))))
		return NULL;
	EVBUFFER_LOCK(buffer);
	e->cb.cb_func = cb;
	e->cbarg = cbarg;
	e->flags = EVBUFFER_CB_ENABLED;//允许回调
	TAILQ_INSERT_HEAD(&buffer->callbacks, e, next);
	EVBUFFER_UNLOCK(buffer);
	return e;
}


//在链表头添加数据：
//前面的evbuffer_add是在链表尾部追加数据，
//Libevent提供了另外一个函数evbuffer_prepend可以在链表头部添加数据。
//在这个函数里面可以看到evbuffer_chain结构体成员misalign的一些使用，
//也能知道为什么会有这个成员。

//evbuffer_prepend函数并不复杂，
//只需弄懂misalign的作用就很容易明白该函数的实现。
//考虑这种情况：要在链表头插入数据，那么应该new一个新的evbuffer_chain，
//然后把要插入的数据放到这个新建个的evbuffer_chain中。
//但evbuffer_chain_new申请到的buffer空间可能会大于要插入的数据长度。
//插入数据后，buffer就必然会剩下一些空闲空间。
//那么这个空闲空间放在buffer的前面好还是后面好呢？
//Libevent认为放在前面会好些，此时misalign就有用了。
//它表示错开不用的空间，也就是空闲空间。
//如果再次在链表头插入数据，就可以使用到这些空闲空间了。
//所以，misalign也可以认为是空闲空间，可以随时使用。
int
evbuffer_prepend(struct evbuffer *buf, const void *data, size_t datlen)
{
	struct evbuffer_chain *chain, *tmp;
	int result = -1;

	EVBUFFER_LOCK(buf);

	if (buf->freeze_start) {
		goto done;
	}

	chain = buf->first;

    //该链表暂时还没有节点  
	if (chain == NULL) {
		chain = evbuffer_chain_new(datlen);
		if (!chain)
			goto done;
		evbuffer_chain_insert(buf, chain);
	}

	/* we cannot touch immutable buffers */
	if ((chain->flags & EVBUFFER_IMMUTABLE) == 0) {//该chain可以修改
		/* If this chain is empty, we can treat it as
		 * 'empty at the beginning' rather than 'empty at the end' */
		if (chain->off == 0)
			chain->misalign = chain->buffer_len;

        //考虑这种情况:一开始chain->off等于0，之后调用evbuffer_prepend插入  
        //一些数据(还没填满这个chain),之后再次调用evbuffer_prepend插入一些  
        //数据。这样就能分别进入下面的if else了  
		if ((size_t)chain->misalign >= datlen) {//空闲空间足够大
			/* we have enough space to fit everything */
			memcpy(chain->buffer + chain->misalign - datlen,
			    data, datlen);
			chain->off += datlen;
			chain->misalign -= datlen;
			buf->total_len += datlen;
			buf->n_add_for_cb += datlen;
			goto out;
		} else if (chain->misalign) {//不够大，但也要用
			/* we can only fit some of the data. */
			memcpy(chain->buffer,//用完这个chain,所以从头开始
			    (char*)data + datlen - chain->misalign,
			    (size_t)chain->misalign);
			chain->off += (size_t)chain->misalign;
			buf->total_len += (size_t)chain->misalign;
			buf->n_add_for_cb += (size_t)chain->misalign;
			datlen -= (size_t)chain->misalign;
			chain->misalign = 0;
		}
	}

    //为datlen申请一个evbuffer_chain。把datlen长的数据放到这个新建的chain  
	/* we need to add another chain */
	if ((tmp = evbuffer_chain_new(datlen)) == NULL)
		goto done;
	buf->first = tmp;
	if (buf->last_with_datap == &buf->first)
		buf->last_with_datap = &tmp->next;

	tmp->next = chain;

	tmp->off = datlen;
	tmp->misalign = tmp->buffer_len - datlen;

	memcpy(tmp->buffer + tmp->misalign, data, datlen);
	buf->total_len += datlen;
	buf->n_add_for_cb += (size_t)chain->misalign;

out:
	evbuffer_invoke_callbacks(buf);
	result = 0;
done:
	EVBUFFER_UNLOCK(buf);
	return result;
}//end evbuffer_prepend



int
evbuffer_remove_cb_entry(struct evbuffer *buffer,
			 struct evbuffer_cb_entry *ent)
{
	EVBUFFER_LOCK(buffer);
	TAILQ_REMOVE(&buffer->callbacks, ent, next);
	EVBUFFER_UNLOCK(buffer);
	mm_free(ent);
	return 0;
}

int
evbuffer_remove_cb(struct evbuffer *buffer, evbuffer_cb_func cb, void *cbarg)
{
	struct evbuffer_cb_entry *cbent;
	int result = -1;
	EVBUFFER_LOCK(buffer);
	TAILQ_FOREACH(cbent, &buffer->callbacks, next) {
		if (cb == cbent->cb.cb_func && cbarg == cbent->cbarg) {
			result = evbuffer_remove_cb_entry(buffer, cbent);
			goto done;
		}
	}
done:
	EVBUFFER_UNLOCK(buffer);
	return result;
}

int
evbuffer_cb_set_flags(struct evbuffer *buffer,
		      struct evbuffer_cb_entry *cb, ev_uint32_t flags)
{
	/* the user isn't allowed to mess with these. */
	flags &= ~EVBUFFER_CB_INTERNAL_FLAGS;
	EVBUFFER_LOCK(buffer);
	cb->flags |= flags;
	EVBUFFER_UNLOCK(buffer);
	return 0;
}

int
evbuffer_cb_clear_flags(struct evbuffer *buffer,
		      struct evbuffer_cb_entry *cb, ev_uint32_t flags)
{
	/* the user isn't allowed to mess with these. */
	flags &= ~EVBUFFER_CB_INTERNAL_FLAGS;
	EVBUFFER_LOCK(buffer);
	cb->flags &= ~flags;
	EVBUFFER_UNLOCK(buffer);
	return 0;
}


int
evbuffer_freeze(struct evbuffer *buffer, int start)
{
	EVBUFFER_LOCK(buffer);
	if (start)
		buffer->freeze_start = 1;
	else
		buffer->freeze_end = 1;
	EVBUFFER_UNLOCK(buffer);
	return 0;
}

int
evbuffer_unfreeze(struct evbuffer *buffer, int start)
{
	EVBUFFER_LOCK(buffer);
	if (start)
		buffer->freeze_start = 0;
	else
		buffer->freeze_end = 0;
	EVBUFFER_UNLOCK(buffer);
	return 0;
}

