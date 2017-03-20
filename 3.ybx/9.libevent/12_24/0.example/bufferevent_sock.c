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

//由于有几个不同类型的bufferevent，而且它们的enable、disable等操作是不同的。所以  
//需要的一些函数指针指明某个类型的bufferevent应该使用哪些操作函数。结构体bufferevent_ops_socket  
//就应运而生。
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

//这个函数首先进行一些判断，满足条件后就会把这个监听写事件的event添加到event_base中。
//其中event_pending函数就是判断这个bufev->ev_write是否已经被event_base删除了。
static void
bufferevent_socket_outbuf_cb(struct evbuffer *buf,
    const struct evbuffer_cb_info *cbinfo,
    void *arg)
{
	struct bufferevent *bufev = arg;
	struct bufferevent_private *bufev_p =
	    EVUTIL_UPCAST(bufev, struct bufferevent_private, bev);

	if (cbinfo->n_added && //evbuffer添加了数据 
	    (bufev->enabled & EV_WRITE) && //默认情况下是enable EV_WRITE的
	    !event_pending(&bufev->ev_write, EV_WRITE, NULL) && //这个event已经被踢出event_base了
	    !bufev_p->write_suspended) {//这个bufferevent的写并没有被挂起
		//把这个event添加到event_base中
		/* Somebody added data to the buffer, and we would like to
		 * write, and we were not writing.  So, start writing. */
		if (be_socket_add(&bufev->ev_write, &bufev->timeout_write) == -1) {
		    /* Should we log this? */
		}
	}
}


//从socket中读取数据：
//从前面的一系列博文可以知道，如果一个socket可读了，那么监听可读事件的event的回调函数就会被调用。
//这个回调函数是在bufferevent_socket_new函数中被Libevent内部设置的，设置为bufferevent_readcb函数，用户并不知情。
//当socket有数据可读时，Libevent就会监听到，然后调用bufferevent_readcb函数处理。
//该函数会调用evbuffer_read函数，把数据从socket fd中读取到evbuffer中。
//然后再调用用户在bufferevent_setcb函数中设置的读事件回调函数。
//所以，当用户的读事件回调函数被调用时，数据已经在evbuffer中了，用户拿来就用，无需调用read这类会阻塞的函数。

//细心的读者可能会发现：对用户的读事件回调函数的触发是边缘触发的。
//这也就要求，在回调函数中，用户应该尽可能地把evbuffer的所有数据都读出来。
//如果想等到下一次回调时再读，那么需要等到下一次socketfd接收到数据才会触发用户的回调函数。
//如果之后socket fd一直收不到任何数据，那么即使evbuffer还有数据，用户的回调函数也不会被调用了。
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

	//用户设置了高水位 
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
    //因为用户可以限速，所以这么要检测最大的可读大小。  
    //如果没有限速的话，那么将返回16384字节，即16K  
    //默认情况下是没有限速的。
	readmax = _bufferevent_get_read_max(bufev_p);
	if (howmuch < 0 || howmuch > readmax) /* The use of -1 for "unlimited"
					       * uglifies this code. XXXX */
		howmuch = readmax;

    //一些原因导致读 被挂起，比如加锁了。  
	if (bufev_p->read_suspended)
		goto done;

    //解冻，使得可以在input的后面追加数据  
	evbuffer_unfreeze(input, 0);
	res = evbuffer_read(input, fd, (int)howmuch); /* XXXX evbuffer_read would do better to take and return ev_ssize_t */
	evbuffer_freeze(input, 0);

	if (res == -1) {
		int err = evutil_socket_geterror(fd);
		if (EVUTIL_ERR_RW_RETRIABLE(err))
			goto reschedule;

        //不是 EINTER or EAGAIN 这两个可以重试的错误，那么就应该是其他致命的错误  
        //此时，应该报告给用户 
		/* error case */
		what |= BEV_EVENT_ERROR;
	} else if (res == 0) {//断开了连接
		/* eof case */
		what |= BEV_EVENT_EOF;
	}

	if (res <= 0)
		goto error;

    //速率相关的操作  
	_bufferevent_decrement_read_buckets(bufev_p, res);

	//evbuffer的数据量大于低水位值。  
	/* Invoke the user callback - must always be called last */
	if (evbuffer_get_length(input) >= bufev->wm_read.low)
		_bufferevent_run_readcb(bufev);//调用用户设置的回调函数

	goto done;

 reschedule:
	goto done;

 error:
    //把监听可读事件的event从event_base的事件队列中删除掉.event_del  
    bufferevent_disable(bufev, EV_READ);//会调用be_socket_disable函数  
    _bufferevent_run_eventcb(bufev, what);//会调用用户设置的错误处理函数  


 done:
	_bufferevent_decref_and_unlock(bufev);
}//end bufferevent_readcb


//处理写事件：
//=========================================================================================
//可读事件:
//进行监听是比较容易的，但对于一个可写事件进行监听则比较困难。
//为什么呢？因为可读监听是监听fd的读缓冲区是否有数据了，如果没有数据那么就一直等待。
//=========================================================================================
//可写事件:
//首先要明白“什么是可写”，可写就是fd的写缓冲区(这个缓冲区在内核)还没满，可以往里面放数据。
//这就有一个问题，如果写缓冲区没有满，那么就一直是可写状态。
//如果一个event监听了可写事件，那么这个event就会一直被触发（死循环）。
//因为一般情况下，如果不是发大量的数据这个写缓冲区是不会满的。
//=========================================================================================
//也就是说，不能监听可写事件。但我们确实要往fd中写数据，那怎么办？
//Libevent的做法是：当我们确实要写入数据时，才监听可写事件。
//也就是说我们调用bufferevent_write写入数据时，Libevent才会把监听可写事件的那个event注册到event_base中。
//当Libevent把数据都写入到fd的缓冲区后，Libevent又会把这个event从event_base中删除。比较烦琐。        
//=========================================================================================
//bufferevent_writecb函数不仅仅要处理上面说到的那个问题，还要处理另外一个坑爹的问题。
//判断socket fd是不是已经连接上服务器了。
//这是因为这个socket fd是非阻塞的，所以它调用connect时，可能还没连接上就返回了。
//对于非阻塞socketw fd，一般是通过判断这个socket是否可写，从而得知这个socket是否已经连接上服务器。
//如果可写，那么它就已经成功连接上服务器了。这个问题，这里先提一下，后面会详细讲。
//=========================================================================================
//同前面的监听可读一样，Libevent是在bufferevent_socket_new函数设置可写的回调函数，为bufferevent_writecb。
//逻辑比较清晰，调用evbuffer_write_atmost函数把数据从evbuffer中写到socket fd缓冲区中，
//此时要注意函数的返回值，因为可能写的时候发生错误。
//=========================================================================================
//之后，还要判断evbuffer的数据是否已经全部写到socket fd的缓冲区了。
//如果已经全部写了，那么就要把监听写事件的event从event_base的插入队列中删除。
//如果还没写完，那么就不能删除，因为还要继续监听可写事件，下次接着写。

//如果已经连接上了，那么会调用用户设置event回调函数(网上也称之为错误处理函数)，通知用户已经连接上了。
//并且，还会把监听可写事件的event从event_base中删除，其理由在前面已经说过了。
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

	//正在连接。因为这个sockfd可能是非阻塞的，所以可能之前的connect还没  
    //连接上。而判断该sockfd是否成功连接上了的一个方法是判断这个sockfd是否可写  
	if (bufev_p->connecting) {
		//c等于1，说明已经连接成功  
        //c等于0，说明还没连接上  
        //c等于-1，说明发生错误  
		int c = evutil_socket_finished_connecting(fd);
		/* we need to fake the error if the connection was refused
		 * immediately - usually connection to localhost on BSD */
		if (bufev_p->connection_refused) {//在bufferevent_socket_connect中被设置
		  bufev_p->connection_refused = 0;
		  c = -1;
		}

		if (c == 0)//还没连接上，继续监听可写吧
			goto done;

        //错误，或者已经连接上了  
        bufev_p->connecting = 0;//修改标志值  

		if (c < 0) {////错误
			event_del(&bufev->ev_write);
			event_del(&bufev->ev_read);
			goto done;
		} else {//连接上了
			connected = 1;
			//会调用用户设置的错误处理函数。太神奇了  
			_bufferevent_run_eventcb(bufev,
					BEV_EVENT_CONNECTED);
			if (!(bufev->enabled & EV_WRITE) ||//默认都是enable EV_WRITE的  
			    bufev_p->write_suspended) {
				event_del(&bufev->ev_write);////不再需要监听可写。因为已经连接上了 
				goto done;
			}
		}
	}

    //用户可能设置了限速，如果没有限速，那么atmost将返回16384(16K)  
	atmost = _bufferevent_get_write_max(bufev_p);

    //一些原因导致写被挂起来了  
	if (bufev_p->write_suspended)
		goto done;

    //如果evbuffer有数据可以写到sockfd中  
	if (evbuffer_get_length(bufev->output)) {
        //解冻链表头  
		evbuffer_unfreeze(bufev->output, 1);
        //将output这个evbuffer的数据写到socket fd的缓冲区中  
        //会把已经写到socket fd缓冲区的数据，从evbuffer中删除 
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

    //如果把写缓冲区的数据都写完成了。为了防止event_base不断地触发可写  
    //事件，此时要把这个监听可写的event删除。  
    //前面的atmost限制了一次最大的可写数据。如果还没写所有的数据  
    //那么就不能delete这个event，而是要继续监听可写事情，知道把所有的  
    //数据都写到socket fd中。 
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
	bufferevent_disable(bufev, EV_WRITE); //有错误。把这个写event删除
	_bufferevent_run_eventcb(bufev, what);

 done:
	_bufferevent_decref_and_unlock(bufev);
}//end bufferevent_writecb

//函数在最后面会冻结两个缓冲区。
//其实，虽然这里冻结了，但实际上Libevent在读数据或者写数据之前会解冻的读完或者写完数据后，又会马上冻结。
//这主要防止数据被意外修改。用户一般不会直接调用evbuffer_freeze或者evbuffer_unfreeze函数。
//一切的冻结和解冻操作都由Libevent内部完成。还有一点要注意，因为这里只是把写缓冲区的头部冻结了。
//所以还是可以往写缓冲区的尾部追加数据。同样，此时也是可以从读缓冲区读取数据。这个是必须的。
//因为在Libevent内部不解冻的时候，用户需要从读缓冲区中获取数据(这相当于从socket fd中读取数据)，
//用户也需要把数据写到写缓冲区中(这相当于把数据写入到socket fd中)。
struct bufferevent *
bufferevent_socket_new(struct event_base *base, evutil_socket_t fd,
    int options)
{
	struct bufferevent_private *bufev_p;
	struct bufferevent *bufev;

    //结构体内存清零，所有成员都为0  
	if ((bufev_p = mm_calloc(1, sizeof(struct bufferevent_private)))== NULL)
		return NULL;

    //如果options中需要线程安全，那么就会申请锁  
    //会新建一个输入和输出缓存区 
	if (bufferevent_init_common(bufev_p, base, &bufferevent_ops_socket,
				    options) < 0) {
		mm_free(bufev_p);
		return NULL;
	}
	bufev = &bufev_p->bev;
    //设置将evbuffer的数据向fd传  
	evbuffer_set_flags(bufev->output, EVBUFFER_FLAG_DRAINS_TO_FD);

    //将fd与event相关联。同一个fd关联两个event  
	event_assign(&bufev->ev_read, bufev->ev_base, fd,
	    EV_READ|EV_PERSIST, bufferevent_readcb, bufev);
	event_assign(&bufev->ev_write, bufev->ev_base, fd,
	    EV_WRITE|EV_PERSIST, bufferevent_writecb, bufev);

    //设置evbuffer的回调函数，使得外界给写缓冲区添加数据时，能触发  
    //写操作，这个回调对于写事件的监听是很重要的 
	evbuffer_add_cb(bufev->output, bufferevent_socket_outbuf_cb, bufev);

    //冻结读缓冲区的尾部，未解冻之前不能往读缓冲区追加数据  
    //也就是说不能从socket fd中读取数据 
	evbuffer_freeze(bufev->input, 0);//0表示尾
    //冻结写缓冲区的头部，未解冻之前不能把写缓冲区的头部数据删除  
    //也就是说不能把数据写到socket fd  
	evbuffer_freeze(bufev->output, 1);//1表示头

	return bufev;
}// end bufferevent_socket_new


//9.libevent/0.use_libevent/1.client.c

//bufferevent_socket_connect的使用：
//用户可以在调用bufferevent_socket_new函数时，传一个-1作为socket的文件描述符，
//然后调用bufferevent_socket_connect函数连接服务器，无需自己写代码调用connect函数连接服务器。
//
//bufferevent_socket_connect函数会调用socket函数申请一个套接字fd，
//然后把这个fd设置成非阻塞的(这就导致了一些坑爹的事情)。
//接着就connect服务器，因为该socket fd是非阻塞的，所以不会等待，而是马上返回，连接这工作交给内核来完成。
//所以，返回后这个socket还没有真正连接上服务器。那么什么时候连接上呢？内核又是怎么通知通知用户呢？
//
//一般来说，当可以往socket fd写东西了，那就说明已经连接上了。也就是说这个socket fd变成可写状态，就连接上了。
//所以，对于“非阻塞connect”比较流行的做法是：用select或者poll这类多路IO复用函数监听该socket的可写事件。
//当这个socket触发了可写事件，然后再对这个socket调用getsockopt函数，做进一步的判断。
//Libevent也是这样实现的，下面来看一下bufferevent_socket_connect函数。

//这个函数比较多错误处理的代码，大致看一下就行了。
//有几个地方要注意，即使connect的时候被拒绝，或者已经连接上了，都会手动激活这个event。
//一个event即使没有加入event_base，也是可以手动激活的。具体原理参考这里。

//无论是手动激活event，或者监听到这个event可写了，都是会调用bufferevent_writecb函数。现在再次看一下该函数，只看connect部分。
int
bufferevent_socket_connect(struct bufferevent *bev,
    struct sockaddr *sa, int socklen)
{
	struct bufferevent_private *bufev_p =
	    EVUTIL_UPCAST(bev, struct bufferevent_private, bev);

	evutil_socket_t fd;
	int r = 0;
	int result=-1;
	int ownfd = 0;

	_bufferevent_incref_and_lock(bev);

	if (!bufev_p)
		goto done;

	fd = bufferevent_getfd(bev);
	if (fd < 0) {//该bufferevent还没有设置fd
		if (!sa)
			goto done;
		fd = socket(sa->sa_family, SOCK_STREAM, 0);
		if (fd < 0)
			goto done;
		if (evutil_make_socket_nonblocking(fd)<0)//非阻塞connect
			goto done;
		ownfd = 1;
	}
	if (sa) {
		r = evutil_socket_connect(&fd, sa, socklen);//非阻塞connect
		if (r < 0)
			goto freesock;
	}
    //为bufferevent里面的两个event设置监听的fd  
    //后面会调用bufferevent_enable 
	bufferevent_setfd(bev, fd);
    if (r == 0) {//暂时还没连接上，因为fd是非阻塞的  
        //此时需要监听可写事件，当可写了，并且没有错误的话，就成功连接上了  
        if (! be_socket_enable(bev, EV_WRITE)) {  
            bufev_p->connecting = 1;//标志这个sockfd正在连接  
            result = 0;  
            goto done;  
        }  
    } else if (r == 1) {//已经连接上了  
        /* The connect succeeded already. How very BSD of it. */  
        result = 0;  
        bufev_p->connecting = 1;   
        event_active(&bev->ev_write, EV_WRITE, 1);//手动激活这个event  
    } else {// connection refused  
        /* The connect failed already.  How very BSD of it. */  
        bufev_p->connection_refused = 1;  
        bufev_p->connecting = 1;  
        result = 0;  
        event_active(&bev->ev_write, EV_WRITE, 1);//手动激活这个event  
    }  
	goto done;

freesock:
	_bufferevent_run_eventcb(bev, BEV_EVENT_ERROR);
	if (ownfd)
		evutil_closesocket(fd);
	/* do something about the error? */
done:
	_bufferevent_decref_and_unlock(bev);
	return result;
}//end bufferevent_socket_connect

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

//居然是直接删除这个监听读事件的event，真的是挂了!!!
//看来不能随便设置高水位，因为它会暂停读。
//如果只想设置低水位而不想设置高水位，
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
