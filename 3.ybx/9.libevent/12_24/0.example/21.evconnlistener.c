#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <event3/util.h>
#include <event3/event.h>
#include "mm-internal.h" //此头本应是libvent内部使用 在这是测试下
#include "log-internal.h" //此头本应是libvent内部使用 在这是测试下
#include "event-internal.h"
#include <event3/listener.h>
#include <event3/bufferevent.h>
#include <event3/thread.h>

#include <pthread.h> //Linux thread

//使用evconnlistener:
//基于event和event_base已经可以写一个CS模型了。
//但是对于服务器端来说，仍然需要用户自行调用socket、bind、listen、accept等步骤。
//这个过程有点繁琐，为此在2.0.2-alpha版本的Libevent推出了一些对应的封装函数。
//用户只需初始化struct sockaddr_in结构体变量，然后把它作为参数传给函数evconnlistener_new_bind即可。
//该函数会完成上面说到的那4个过程。下面的代码是一个使用例子。

//从上面代码可以看到，当服务器端监听到一个客户端的连接请求后，就会调用listener_cb这个回调函数。
//这个回调函数是在evconnlistener_new_bind函数中设置的。
//现在来看一下这个函数的参数有哪些，下面是其函数原型。

void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
                 struct sockaddr *sock, int socklen, void *arg);

void socket_read_cb(struct bufferevent *bev, void *arg);
void socket_error_cb(struct bufferevent *bev, short events, void *arg);

//客户请求方法1:
//nc 127.0.0.1 8989
//=================
//客户请求方法2:
int main()
{
    evthread_use_pthreads();//enable threads

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(struct sockaddr_in));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8989);

    struct event_base *base = event_base_new();
    struct evconnlistener *listener
            = evconnlistener_new_bind(base, listener_cb, base,
                                      LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE | LEV_OPT_THREADSAFE,
                                      10, (struct sockaddr*)&sin,
                                      sizeof(struct sockaddr_in));

    event_base_dispatch(base);

	evconnlistener_free(listener);
    event_base_free(base);

    return 0;
}

//有新的客户端连接到服务器
//当此函数被调用时，libevent已经帮我们accept了这个客户端。该客户端的
//文件描述符为fd
void listener_cb(struct evconnlistener *listener, evutil_socket_t fd, 
		struct sockaddr *sock, int socklen, void *arg)
{
    struct event_base *base = (struct event_base*)arg;

	//下面代码是为这个fd创建一个bufferevent
    struct bufferevent *bev =  bufferevent_socket_new(base, fd,
                                               BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(bev, socket_read_cb, NULL, socket_error_cb, NULL);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);
}


void socket_read_cb(struct bufferevent *bev, void *arg)
{
    char msg[4096];

    size_t len = bufferevent_read(bev, msg, sizeof(msg)-1 );

    msg[len] = '\0';
    printf("server read the data %s\n", msg);

    char reply[] = "I has read your data";
    bufferevent_write(bev, reply, strlen(reply) );
}


void socket_error_cb(struct bufferevent *bev, short events, void *arg)
{
    if (events & BEV_EVENT_EOF)
        printf("connection closed\n");
    else if (events & BEV_EVENT_ERROR)
        printf("some other error\n");

    //这将自动close套接字和free读写缓冲区
    bufferevent_free(bev);
}
