#ifndef _EVENT2_LISTENER_H_
#define _EVENT2_LISTENER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <event3/event.h>

struct sockaddr;
struct evconnlistener;

typedef void (*evconnlistener_cb)(struct evconnlistener *, evutil_socket_t, struct sockaddr *, int socklen, void *);
typedef void (*evconnlistener_errorcb)(struct evconnlistener *, void *);


//LEV_OPT_LEAVE_SOCKETS_BLOCKING:
//默认情况下，当连接监听器接收到新的客户端socket连接后，会把该socket设置为非阻塞的。
//如果设置该选项，那么就把之客户端socket保留为阻塞的
//
//LEV_OPT_CLOSE_ON_FREE:
//当连接监听器释放时，会自动关闭底层的socket
//
//LEV_OPT_CLOSE_ON_EXEC:
//为底层的socket设置close-on-exec标志
//
//LEV_OPT_REUSEABLE:
// 在某些平台，默认情况下当一个监听socket被关闭时，其他socket不能马上绑定到同一个端口，要等一会儿才行。
//设置该标志后，Libevent会把该socket设置成reuseable。
//这样，关闭该socket后，其他socket就能马上使用同一个端口
//
//LEV_OPT_THREADSAFE:
//为连接监听器分配锁。这样可以确保线程安全
/** Flag: Indicates that we should not make incoming sockets nonblocking
 * before passing them to the callback. */
#define LEV_OPT_LEAVE_SOCKETS_BLOCKING	(1u<<0)
/** Flag: Indicates that freeing the listener should close the underlying
 * socket. */
#define LEV_OPT_CLOSE_ON_FREE		(1u<<1)
/** Flag: Indicates that we should set the close-on-exec flag, if possible */
#define LEV_OPT_CLOSE_ON_EXEC		(1u<<2)
/** Flag: Indicates that we should disable the timeout (if any) between when
 * this socket is closed and when we can listen again on the same port. */
#define LEV_OPT_REUSEABLE		(1u<<3)
/** Flag: Indicates that the listener should be locked so it's safe to use
 * from multiple threadcs at once. */
#define LEV_OPT_THREADSAFE		(1u<<4)

struct evconnlistener *evconnlistener_new(struct event_base *base,
    evconnlistener_cb cb, void *ptr, unsigned flags, int backlog,
    evutil_socket_t fd);

struct evconnlistener *evconnlistener_new_bind(struct event_base *base,
    evconnlistener_cb cb, void *ptr, unsigned flags, int backlog,
    const struct sockaddr *sa, int socklen);

void evconnlistener_free(struct evconnlistener *lev);
int evconnlistener_enable(struct evconnlistener *lev);
int evconnlistener_disable(struct evconnlistener *lev);
struct event_base *evconnlistener_get_base(struct evconnlistener *lev);
evutil_socket_t evconnlistener_get_fd(struct evconnlistener *lev);

#ifdef __cplusplus
}
#endif

#endif
