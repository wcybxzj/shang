#include <event3/event-config.h>

#include <sys/types.h>
#ifdef _EVENT_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <sys/queue.h>
#ifdef _EVENT_HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _EVENT_HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>
#ifdef _EVENT_HAVE_FCNTL_H
#include <fcntl.h>
#endif


#include <event3/event.h>
#include <event3/event_struct.h>
#include "event-internal.h"
#include <event3/util.h>
#include "evsignal-internal.h"
#include "log-internal.h"
#include "evmap-internal.h"
#include "evthread-internal.h"


#ifndef WIN32
/* Windows wants us to call our signal handlers as __cdecl.  Nobody else
 * expects you to do anything crazy like this. */
#define __cdecl
#endif

static int evsig_add(struct event_base *, evutil_socket_t, short, short, void *);
static int evsig_del(struct event_base *, evutil_socket_t, short, short, void *);

static const struct eventop evsigops = {
	"signal",
	NULL,
	evsig_add,
	evsig_del,
	NULL,
	NULL,
	0, 0, 0
};

#ifndef _EVENT_DISABLE_THREAD_SUPPORT
/* Lock for evsig_base and evsig_base_n_signals_added fields. */
static void *evsig_base_lock = NULL;
#endif

static struct event_base *evsig_base = NULL;
/* A copy of evsig_base->sigev_n_signals_added. */
static int evsig_base_n_signals_added = 0;
static evutil_socket_t evsig_base_fd = -1;

static void __cdecl evsig_handler(int sig);

#define EVSIGBASE_LOCK() EVLOCK_LOCK(evsig_base_lock, 0)
#define EVSIGBASE_UNLOCK() EVLOCK_UNLOCK(evsig_base_lock, 0)

void
evsig_set_base(struct event_base *base)
{
	EVSIGBASE_LOCK();
	evsig_base = base;
	evsig_base_n_signals_added = base->sig.ev_n_signals_added;
	evsig_base_fd = base->sig.ev_signal_pair[0];
	EVSIGBASE_UNLOCK();
}

/* Callback for when the signal handler write a byte to our signaling socket */
static void  
evsig_cb(evutil_socket_t fd, short what, void *arg)  
{  
    static char signals[1024];  
    ev_ssize_t n;  
    int i;  
  
    //NSIG是信号的个数      
    int ncaught[NSIG];  
    struct event_base *base;  
  
    base = arg;  
  
    memset(&ncaught, 0, sizeof(ncaught));  
  
    while (1) {  
        //读取socketpair中的数据。从中可以知道有哪些信号发生了  
        //已经socketpair的读端已经设置为非阻塞的。所以不会被阻塞在  
        //recv函数中。这个循环要把socketpair的所有数据都读取出来  
        n = recv(fd, signals, sizeof(signals), 0);  
        if (n == -1) {  
            int err = evutil_socket_geterror(fd);  
            if (! EVUTIL_ERR_RW_RETRIABLE(err))  
                event_sock_err(1, fd, "%s: recv", __func__);  
            break;  
        } else if (n == 0) {  
            /* XXX warn? */  
            break;  
        }  
  
        //遍历数据数组，把每一个字节当作一个信号  
        for (i = 0; i < n; ++i) {  
            ev_uint8_t sig = signals[i];  
            if (sig < NSIG)  
                ncaught[sig]++; //该信号发生的次数  
        }  
    }  
  
    EVBASE_ACQUIRE_LOCK(base, th_base_lock);  
    for (i = 0; i < NSIG; ++i) {  
        if (ncaught[i]) //有信号发生就为之调用evmap_signal_active  
            evmap_signal_active(base, i, ncaught[i]);  
    }  
    EVBASE_RELEASE_LOCK(base, th_base_lock);  
}  //end evsig_cb

int  
evsig_init(struct event_base *base)  
{  
    //创建一个socketpair  
    if (evutil_socketpair(  
            AF_UNIX, SOCK_STREAM, 0, base->sig.ev_signal_pair) == -1) {  
#ifdef WIN32  
        /* Make this nonfatal on win32, where sometimes people 
           have localhost firewalled. */  
        event_sock_warn(-1, "%s: socketpair", __func__);  
#else  
        event_sock_err(1, -1, "%s: socketpair", __func__);  
#endif  
        return -1;  
    }  
  
    //子进程不能访问该socketpair  
    evutil_make_socket_closeonexec(base->sig.ev_signal_pair[0]);  
    evutil_make_socket_closeonexec(base->sig.ev_signal_pair[1]);  
    base->sig.sh_old = NULL;  
    base->sig.sh_old_max = 0;  
  
    evutil_make_socket_nonblocking(base->sig.ev_signal_pair[0]);  
    evutil_make_socket_nonblocking(base->sig.ev_signal_pair[1]);  
  
    //将ev_signal_pair[1]与ev_signal这个event相关联。ev_signal_pair[1]为读端  
    //该函数的作用等同于event_new。
	//实际上event_new内部也是调用event_assign函数完成工作的  
    event_assign(&base->sig.ev_signal, base, base->sig.ev_signal_pair[1],  
        EV_READ | EV_PERSIST, evsig_cb, base);  
  
    //标明是内部使用的  
    base->sig.ev_signal.ev_flags |= EVLIST_INTERNAL;  
    //Libevent中，event是有优先级的。前一篇博文已经说到这一点  
    event_priority_set(&base->sig.ev_signal, 0); //最高优先级  
  
    base->evsigsel = &evsigops;  
  
    return 0;  
}// end evsg_init  

/* Helper: set the signal handler for evsignal to handler in base, so that
 * we can restore the original handler when we clear the current one. */
int
_evsig_set_handler(struct event_base *base,
    int evsignal, void (__cdecl *handler)(int))
{
#ifdef _EVENT_HAVE_SIGACTION
	struct sigaction sa;
#else
	ev_sighandler_t sh;
#endif
	struct evsig_info *sig = &base->sig;
	void *p;

	/*
	 * resize saved signal handler array up to the highest signal number.
	 * a dynamic array is used to keep footprint on the low side.
	 */
	//数组的一个元素就存放一个信号。信号值等于其下标  
    //信号值等于其数组下标  
	if (evsignal >= sig->sh_old_max) {
		int new_max = evsignal + 1;
		event_debug(("%s: evsignal (%d) >= sh_old_max (%d), resizing",
			    __func__, evsignal, sig->sh_old_max));
		p = mm_realloc(sig->sh_old, new_max * sizeof(*sig->sh_old));
		if (p == NULL) {
			event_warn("realloc");
			return (-1);
		}

		memset((char *)p + sig->sh_old_max * sizeof(*sig->sh_old),
		    0, (new_max - sig->sh_old_max) * sizeof(*sig->sh_old));

		sig->sh_old_max = new_max;
		sig->sh_old = p;
	}
	//注意sh_old是一个二级指针。元素是一个一级指针。为这个一级指针分配内存  
	/* allocate space for previous handler out of dynamic array */
	sig->sh_old[evsignal] = mm_malloc(sizeof *sig->sh_old[evsignal]);
	if (sig->sh_old[evsignal] == NULL) {
		event_warn("malloc");
		return (-1);
	}

	/* save previous handler and setup new handler */
#ifdef _EVENT_HAVE_SIGACTION
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handler;
	sa.sa_flags |= SA_RESTART;
	sigfillset(&sa.sa_mask);

	if (sigaction(evsignal, &sa, sig->sh_old[evsignal]) == -1) {
		event_warn("sigaction");
		mm_free(sig->sh_old[evsignal]);
		sig->sh_old[evsignal] = NULL;
		return (-1);
	}
#else
	if ((sh = signal(evsignal, handler)) == SIG_ERR) {
		event_warn("signal");
		mm_free(sig->sh_old[evsignal]);
		sig->sh_old[evsignal] = NULL;
		return (-1);
	}
	//保存之前的信号捕抓函数。当用户event_del这个信号监听后，就可以恢复了  
	*sig->sh_old[evsignal] = sh;
#endif

	return (0);
}//end _evsig_set_handler

static int
evsig_add(struct event_base *base, evutil_socket_t evsignal, short old, short events, void *p)
{
	struct evsig_info *sig = &base->sig;
	(void)p;
    //NSIG是信号的个数。定义在系统头文件中  
	EVUTIL_ASSERT(evsignal >= 0 && evsignal < NSIG);

	/* catch signals if they happen quickly */
    //加锁保护。但实际其锁变量为NULL。所以并没有保护。应该会在以后的版本有所改正  
    //在2.1.4-alpha版本中，就已经改进了这个问题。为锁变量分配了锁  
	EVSIGBASE_LOCK();
	//如果有多个event_base，那么捕抓信号这个工作只能由其中一个完成。  
	if (evsig_base != base && evsig_base_n_signals_added) {
		event_warnx("Added a signal to event base %p with signals "
		    "already added to event_base %p.  Only one can have "
		    "signals at a time with the %s backend.  The base with "
		    "the most recently added signal or the most recent "
		    "event_base_loop() call gets preference; do "
		    "not rely on this behavior in future Libevent versions.",
		    base, evsig_base, base->evsel->name);
	}
	evsig_base = base;
	evsig_base_n_signals_added = ++sig->ev_n_signals_added;
	evsig_base_fd = base->sig.ev_signal_pair[0];
	EVSIGBASE_UNLOCK();

	event_debug(("%s: %d: changing signal handler", __func__, (int)evsignal));
	if (_evsig_set_handler(base, (int)evsignal, evsig_handler) == -1) {
		goto err;
	}

	//从后面的那个if语句可以得知，
	//当sig->ev_signal_added变量为0时(即用户第一次监听一个信号)，
	//就会将ev_signal这个event加入到event_base中。
	//从“统一事件源”可知，ev_signal的作用就是通知event_base，有信号发生。
	//只需一个event即可完成工作，即使用户要监听多个不同的信号，
	//因为这个event已经和socketpair的读端相关联了。
	//如果要监听多个信号，那么就在信号处理函数中往这个socketpair写入不同的值即可。
	//event_base能监听到可读，并可以从读到的内容可以判断是哪个信号发生了。
		   
	//Libevent不会为每个信号监听创建一个event。
	//它只会创建一个全局的专门用于监听信号的event。
	//event_base第一次监听信号事件。要添加ev_signal到event_base中  
	//这里的ev_signal实际是在event_base_new
	if (!sig->ev_signal_added) {
		 //本函数的调用路径为
		//event_add->event_add_internal->evmap_signal_map->evsig_add  
        //所以这里是递归调用event_add函数。
		//而event_add函数是会加锁的。所以需要锁为递归锁  
		//printf("evsignal:%d\n", evsignal);
		if (event_add(&sig->ev_signal, NULL))
			goto err;
		sig->ev_signal_added = 1;
	}

	return (0);

err:
	EVSIGBASE_LOCK();
	--evsig_base_n_signals_added;
	--sig->ev_n_signals_added;
	EVSIGBASE_UNLOCK();
	return (-1);
}// end evsig_add

int
_evsig_restore_handler(struct event_base *base, int evsignal)
{
	int ret = 0;
	struct evsig_info *sig = &base->sig;
#ifdef _EVENT_HAVE_SIGACTION
	struct sigaction *sh;
#else
	ev_sighandler_t *sh;
#endif

	/* restore previous handler */
	sh = sig->sh_old[evsignal];
	sig->sh_old[evsignal] = NULL;
#ifdef _EVENT_HAVE_SIGACTION
	if (sigaction(evsignal, sh, NULL) == -1) {
		event_warn("sigaction");
		ret = -1;
	}
#else
	if (signal(evsignal, *sh) == SIG_ERR) {
		event_warn("signal");
		ret = -1;
	}
#endif

	mm_free(sh);

	return ret;
}



static int
evsig_del(struct event_base *base, evutil_socket_t evsignal, short old, short events, void *p)
{
	EVUTIL_ASSERT(evsignal >= 0 && evsignal < NSIG);

	event_debug(("%s: %d: restoring signal handler", __func__, evsignal));

	EVSIGBASE_LOCK();
	--evsig_base_n_signals_added;
	--base->sig.ev_n_signals_added;
	EVSIGBASE_UNLOCK();

	return (_evsig_restore_handler(base, (int)evsignal));
}

static void __cdecl
evsig_handler(int sig)
{
	int save_errno = errno;
#ifdef WIN32
	int socket_errno = EVUTIL_SOCKET_ERROR();
#endif
	ev_uint8_t msg;

	if (evsig_base == NULL) {
		event_warnx(
			"%s: received signal %d, but have no base configured",
			__func__, sig);
		return;
	}

#ifndef _EVENT_HAVE_SIGACTION
	signal(sig, evsig_handler);
#endif

	/* Wake up our notification mechanism */
	msg = sig;
	send(evsig_base_fd, (char*)&msg, 1, 0);
	errno = save_errno;
#ifdef WIN32
	EVUTIL_SET_SOCKET_ERROR(socket_errno);
#endif
}


void
evsig_dealloc(struct event_base *base)
{
	int i = 0;
	if (base->sig.ev_signal_added) {
		event_del(&base->sig.ev_signal);
		base->sig.ev_signal_added = 0;
	}
	/* debug event is created in evsig_init/event_assign even when
	 * ev_signal_added == 0, so unassign is required */
	event_debug_unassign(&base->sig.ev_signal);

	for (i = 0; i < NSIG; ++i) {
		if (i < base->sig.sh_old_max && base->sig.sh_old[i] != NULL)
			_evsig_restore_handler(base, i);
	}
	EVSIGBASE_LOCK();
	if (base == evsig_base) {
		evsig_base = NULL;
		evsig_base_n_signals_added = 0;
		evsig_base_fd = -1;
	}
	EVSIGBASE_UNLOCK();

	if (base->sig.ev_signal_pair[0] != -1) {
		evutil_closesocket(base->sig.ev_signal_pair[0]);
		base->sig.ev_signal_pair[0] = -1;
	}
	if (base->sig.ev_signal_pair[1] != -1) {
		evutil_closesocket(base->sig.ev_signal_pair[1]);
		base->sig.ev_signal_pair[1] = -1;
	}
	base->sig.sh_old_max = 0;

	/* per index frees are handled in evsig_del() */
	if (base->sig.sh_old) {
		mm_free(base->sig.sh_old);
		base->sig.sh_old = NULL;
	}
}// end evsig_dealloc


#ifndef _EVENT_DISABLE_THREAD_SUPPORT
int
evsig_global_setup_locks_(const int enable_locks)
{
    EVTHREAD_SETUP_GLOBAL_LOCK(evsig_base_lock, 0);
    return 0;
}
#endif

