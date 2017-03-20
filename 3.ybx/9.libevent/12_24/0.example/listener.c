#include <sys/types.h>

#include <event3/event-config.h>

#ifdef WIN32
#ifndef _WIN32_WINNT
/* Minimum required for InitializeCriticalSectionAndSpinCount */
#define _WIN32_WINNT 0x0403
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#endif
#include <errno.h>
#ifdef _EVENT_HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef _EVENT_HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef _EVENT_HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "event3/listener.h"
#include "event3/util.h"
#include "event3/event.h"
#include "event3/event_struct.h"
#include "mm-internal.h"
#include "util-internal.h"
#include "log-internal.h"
#include "evthread-internal.h"
#ifdef WIN32
#include "iocp-internal.h"
#include "defer-internal.h"
#include "event-internal.h"
#endif

//一系列的工作函数
struct evconnlistener_ops {
	int (*enable)(struct evconnlistener *);
	int (*disable)(struct evconnlistener *);
	void (*destroy)(struct evconnlistener *);
	void (*shutdown)(struct evconnlistener *);
	evutil_socket_t (*getfd)(struct evconnlistener *);
	struct event_base *(*getbase)(struct evconnlistener *);
};

struct evconnlistener {  
    const struct evconnlistener_ops *ops;//操作函数  
    void *lock; //锁变量，用于线程安全  
    evconnlistener_cb cb;//用户的回调函数  
    evconnlistener_errorcb errorcb;//发生错误时的回调函数  
    void *user_data;//回调函数的参数  
    unsigned flags;//属性标志  
    short refcnt;//引用计数  
    unsigned enabled : 1;//位域为1.即只需一个比特位来存储这个成员  
}; 

//在evconnlistener_event结构体有一个event结构体。
//可以想象，在实现时必然是将服务器端的socket fd赋值给struct event 类型变量listener的fd成员。
//然后将listener加入到event_base，这样就完成了自动监听工作。这也回归到之前学过的内容。
struct evconnlistener_event {
	struct evconnlistener base;
	struct event listener;//内部event,插入到event_base 
};

#define LOCK(listener) EVLOCK_LOCK((listener)->lock, 0)
#define UNLOCK(listener) EVLOCK_UNLOCK((listener)->lock, 0)

static int event_listener_enable(struct evconnlistener *);
static int event_listener_disable(struct evconnlistener *);
static void event_listener_destroy(struct evconnlistener *);
static evutil_socket_t event_listener_getfd(struct evconnlistener *);
static struct event_base *event_listener_getbase(struct evconnlistener *);

static const struct evconnlistener_ops evconnlistener_event_ops = {
	event_listener_enable,
	event_listener_disable,
	event_listener_destroy,
	NULL, /* shutdown */
	event_listener_getfd,
	event_listener_getbase
};

static int
listener_decref_and_unlock(struct evconnlistener *listener)
{
	int refcnt = --listener->refcnt;
	if (refcnt == 0) {
		//实际调用event_listener_destroy  
		listener->ops->destroy(listener);
		UNLOCK(listener);
		EVTHREAD_FREE_LOCK(listener->lock, EVTHREAD_LOCKTYPE_RECURSIVE);
		mm_free(listener);
		return 1;
	} else {
		UNLOCK(listener);
		return 0;
	}
}

static void
listener_read_cb(evutil_socket_t fd, short what, void *p)
{
	struct evconnlistener *lev = p;
	int err;
	evconnlistener_cb cb;
	evconnlistener_errorcb errorcb;
	void *user_data;
	LOCK(lev);
	while (1) {//可能有多个客户端同时请求连接  
		struct sockaddr_storage ss;
#ifdef WIN32
		int socklen = sizeof(ss);
#else
		socklen_t socklen = sizeof(ss);
#endif
		evutil_socket_t new_fd = accept(fd, (struct sockaddr*)&ss, &socklen);
		if (new_fd < 0){
			//printf("new_fd<0\n");
			break;
		}
		if (socklen == 0) {
			/* This can happen with some older linux kernels in
			 response to nmap. */
			evutil_closesocket(new_fd);
			continue;
		}
		//没设置为阻塞就是accept回来的fd需要时非阻塞的
		if (!(lev->flags & LEV_OPT_LEAVE_SOCKETS_BLOCKING))
			evutil_make_socket_nonblocking(new_fd);

		//用户还没设置连接监听器的回调函数  
		if (lev->cb == NULL) {
			UNLOCK(lev);
			return;
		}

        //由于refcnt被初始化为1.这里有++了，所以一般情况下并不会进入下面的  
        //if判断里面。但如果程在下面UNLOCK之后，第二个线调用evconnlistener_free  
        //释放这个evconnlistener时，就有可能使得refcnt为1了。即进入那个判断体里  
        //执行listener_decref_and_unlock。在下面会讨论这个问题。  

		++lev->refcnt;
		cb = lev->cb;
		user_data = lev->user_data;
		UNLOCK(lev);
		cb(lev, new_fd, (struct sockaddr*)&ss, (int)socklen,
		    user_data);//调用用户设置的回调函数，让用户处理这个fd
		LOCK(lev);
		//现在来说一下那个listener_decref_and_unlock。
		//在函数listener_read_cb中，一般情况下是不会调用listener_decref_and_unlock，但在多线程的时候可能会调用。
		//这种特殊情况是：当主线程accept到一个新客户端时，会解锁，并调用用户设置的回调函数。
		//此时，引用计数等于2。就在这个时候，第二个线程执行evconnlistener_free函数。
		//该函数会执行listener_decref_and_unlock。明显主线程还在用这个evconnlistener，肯定不能删除。
		//此时引用计数也等于2也不会删除。但用户已经调用了evconnlistener_free。Libevent必须要响应。
		//当第二个线程执行完后，主线程抢到CPU，此时引用计数就变成1了，也就进入到if判断里面了。
		//在判断体里面执行函数listener_decref_and_unlock，并且完成删除工作。
		if (lev->refcnt == 1) {
			int freed = listener_decref_and_unlock(lev);
			EVUTIL_ASSERT(freed);
			return;
		}
		--lev->refcnt;
	}
	err = evutil_socket_geterror(fd);
	if (EVUTIL_ERR_ACCEPT_RETRIABLE(err)) {
		UNLOCK(lev);
		return;
	}
	if (lev->errorcb != NULL) {
		++lev->refcnt;
		errorcb = lev->errorcb;
		user_data = lev->user_data;
		UNLOCK(lev);
		errorcb(lev, user_data);
		LOCK(lev);
		listener_decref_and_unlock(lev);
	} else {
		event_sock_warn(fd, "Error from accept() call");
	}
}

static void listener_read_cb(evutil_socket_t, short, void *);

struct evconnlistener *
evconnlistener_new(struct event_base *base,
    evconnlistener_cb cb, void *ptr, unsigned flags, int backlog,
    evutil_socket_t fd)
{
	struct evconnlistener_event *lev;

	if (backlog > 0) {
		if (listen(fd, backlog) < 0)
			return NULL;
	} else if (backlog < 0) {
		if (listen(fd, 128) < 0)
			return NULL;
	}

	lev = mm_calloc(1, sizeof(struct evconnlistener_event));
	if (!lev)
		return NULL;

	lev->base.ops = &evconnlistener_event_ops;
	lev->base.cb = cb;
	lev->base.user_data = ptr;
	lev->base.flags = flags;
	lev->base.refcnt = 1;

	if (flags & LEV_OPT_THREADSAFE) {
		EVTHREAD_ALLOC_LOCK(lev->base.lock, EVTHREAD_LOCKTYPE_RECURSIVE);
	}

	//在多路IO复用函数中，新客户端的连接请求也被当作读事件 
	event_assign(&lev->listener, base, fd, EV_READ|EV_PERSIST,
	    listener_read_cb, lev);

	//会调用event_add，把event加入到event_base中
	evconnlistener_enable(&lev->base);

	return &lev->base;
} // end evconnlistener_new

//第一个参数是很熟悉的event_base，无论怎么样都是离不开event_base这个发动机的。

//第二个参数是一个函数指针，该函数指针的格式如代码所示。
//当有新的客户端请求连接时，该函数就会调用。
//要注意的是：当这个回调函数被调用时，Libevent已经帮我们accept了这个客户端。
//所以，该回调函数有一个参数是文件描述符fd。
//我们直接使用这个fd即可。真是方便。
//这个参数是可以为NULL的，此时用户并不能接收到客户端。
//当用户调用evconnlistener_set_cb函数设置回调函数后，就可以了。

//第三个参数是传给回调函数的用户参数

//参数backlog是系统调用listen的第二个参数。
//最后两个参数就不多说了。

//evconnlistener_new_bind函数申请一个socket，
//然后对之进行一些有关非阻塞、重用、保持连接的处理、绑定到特定的IP和端口。
//最后把业务逻辑交给evconnlistener_new处理。
struct evconnlistener *
evconnlistener_new_bind(struct event_base *base, evconnlistener_cb cb,
    void *ptr, unsigned flags, int backlog, const struct sockaddr *sa,
    int socklen)
{
	struct evconnlistener *listener;
	evutil_socket_t fd;
	int on = 1;
	int family = sa ? sa->sa_family : AF_UNSPEC;

	//监听个数不能为0
	if (backlog == 0)
		return NULL;

	fd = socket(family, SOCK_STREAM, 0);
	if (fd == -1)
		return NULL;

	//LEV_OPT_LEAVE_SOCKETS_BLOCKING选项是应用于accept到的客户端socket  
    //所以对于服务器端的socket，直接将之设置为非阻塞的  
	if (evutil_make_socket_nonblocking(fd) < 0) {
		evutil_closesocket(fd);
		return NULL;
	}

	if (flags & LEV_OPT_CLOSE_ON_EXEC) {
		if (evutil_make_socket_closeonexec(fd) < 0) {
			evutil_closesocket(fd);
			return NULL;
		}
	}

	if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void*)&on, sizeof(on))<0) {
		evutil_closesocket(fd);
		return NULL;
	}
	if (flags & LEV_OPT_REUSEABLE) {
		if (evutil_make_listen_socket_reuseable(fd) < 0) {
			evutil_closesocket(fd);
			return NULL;
		}
	}

	if (sa) {
		if (bind(fd, sa, socklen)<0) {
			evutil_closesocket(fd);
			return NULL;
		}
	}

	listener = evconnlistener_new(base, cb, ptr, flags, backlog, fd);
	if (!listener) {
		evutil_closesocket(fd);
		return NULL;
	}

	return listener;
}//end evconnlistener_new_bind

void
evconnlistener_free(struct evconnlistener *lev)
{
	LOCK(lev);
	lev->cb = NULL;
	lev->errorcb = NULL;
	//这里的shutdown为NULL  
	if (lev->ops->shutdown)
		lev->ops->shutdown(lev);

    //引用次数减一，并解锁
	listener_decref_and_unlock(lev);
}

int
evconnlistener_enable(struct evconnlistener *lev)
{
	int r;
	LOCK(lev);
	lev->enabled = 1;
	//在evconnlistener_enable函数里面，如果用户没有设置回调函数，
	//那么就不会调用event_listener_enable。也就是说并不会add到event_base中。
	if (lev->cb)
		r = lev->ops->enable(lev);//实际上是调用下面的event_listener_enable函数
	else
		r = 0;
	UNLOCK(lev);
	return r;
}

static void
event_listener_destroy(struct evconnlistener *lev)
{
	struct evconnlistener_event *lev_e =
	    EVUTIL_UPCAST(lev, struct evconnlistener_event, base);

	//把event从event_base中删除  
	event_del(&lev_e->listener);
	if (lev->flags & LEV_OPT_CLOSE_ON_FREE)
		//LEV_OPT_CLOSE_ON_FREE选项关闭的是服务器端的监听socket，
		//而非那些连接客户端的socket。
		//如果用户设置了这个选项，那么要关闭socket 
		evutil_closesocket(event_get_fd(&lev_e->listener));
	event_debug_unassign(&lev_e->listener);
}

static int
event_listener_enable(struct evconnlistener *lev)
{
	struct evconnlistener_event *lev_e =
	    EVUTIL_UPCAST(lev, struct evconnlistener_event, base);
	return event_add(&lev_e->listener, NULL);
}

static int
event_listener_disable(struct evconnlistener *lev)
{
	struct evconnlistener_event *lev_e =
	    EVUTIL_UPCAST(lev, struct evconnlistener_event, base);
	return event_del(&lev_e->listener);
}

evutil_socket_t
evconnlistener_get_fd(struct evconnlistener *lev)
{
	evutil_socket_t fd;
	LOCK(lev);
	fd = lev->ops->getfd(lev);
	UNLOCK(lev);
	return fd;
}

static evutil_socket_t
event_listener_getfd(struct evconnlistener *lev)
{
	struct evconnlistener_event *lev_e =
	    EVUTIL_UPCAST(lev, struct evconnlistener_event, base);
	return event_get_fd(&lev_e->listener);
}

struct event_base *
evconnlistener_get_base(struct evconnlistener *lev)
{
	struct event_base *base;
	LOCK(lev);
	base = lev->ops->getbase(lev);
	UNLOCK(lev);
	return base;
}

static struct event_base *
event_listener_getbase(struct evconnlistener *lev)
{
	struct evconnlistener_event *lev_e =
	    EVUTIL_UPCAST(lev, struct evconnlistener_event, base);
	return event_get_base(&lev_e->listener);
}

