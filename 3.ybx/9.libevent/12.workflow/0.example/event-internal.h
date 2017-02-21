#ifndef _EVENT_INTERNAL_H_
#define _EVENT_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <event3/event-config.h>
#include <time.h>
#include <sys/queue.h>
#include <event3/event_struct.h>
#include "minheap-internal.h"
#include "evsignal-internal.h"
#include "mm-internal.h"
#include "defer-internal.h"

/* mutually exclusive */
#define ev_signal_next	_ev.ev_signal.ev_signal_next
#define ev_io_next	_ev.ev_io.ev_io_next
#define ev_io_timeout	_ev.ev_io.ev_timeout

/* used only by signals */
#define ev_ncalls	_ev.ev_signal.ev_ncalls
#define ev_pncalls	_ev.ev_signal.ev_pncalls

/* Possible values for ev_closure in struct event. */
//指定event_base 执行事件处理器的回调函数时的行为
//默认行为
#define EV_CLOSURE_NONE 0
//执行信号事件处理器的回调函数时,调用ev.ev_signal.ev_ncalls次该回调函数
#define EV_CLOSURE_SIGNAL 1
//执行完回调函数后，再次将事件处理器加入注册事件队列中
#define EV_CLOSURE_PERSIST 2

struct eventop {
	//I/O复用名称
    const char *name;
	//为一个事件循环初始化使用该I/O复用机制所需的所有资源，返回指向这些数据的指针，
	//存储在event_base.evbase中，失败返回null
    void *(*init)(struct event_base *);
	//注册事件，event_base表示事件循环，fd表示事件源，old表示该事件源上已注册的方法events表示本次要注册的方法
	//fdinfo表示evmap中一个与事件源相关的结构，长度由fdinfo_len定义，当该事件源是第一次添加时，fdinfo_len为0
	//函数成功返回0，失败返回-1
    int (*add)(struct event_base *, evutil_socket_t fd, short old, short events, void *fdinfo);
	//删除一个已注册的事件
    int (*del)(struct event_base *, evutil_socket_t fd, short old, short events, void *fdinfo);
	// 检测所有注册事件是否已就绪，为每个已就绪事件的event_active方法
    int (*dispatch)(struct event_base *, struct timeval *);
	//释放相关内存
    void (*dealloc)(struct event_base *);
	//标识程序执行fork后是否需要重新初始化
    int need_reinit;
	//I/O技术支持的一些特性，可选如下三个值的按位或：
	//EV_FEATURE_ET(边沿触发事件)，EV_FEATURE_01(事件监测算法是O(1))和
	//EV_FEATURE_FDS(不仅能监听socket上的事件，还能监听其他类型的文件描述符上的事件)
    enum event_method_feature features;
	//有的I/O复用机制需要为每个I/O事件队列和信号事件队列分配额外的内存，
	//以避免同一个文件描述符被重复插入I/O复用机制的事件表中。
	//evmap_io_add(或evmap_io_del)函数在调用eventop的add(或del)方法时，
	//将这段内存的起始地址作为第5个参数传递给add(或del)方法，
	//下面这个成员指定了这段内存的长度
	//额外信息的长度。有些多路IO复用函数需要额外的信息
    size_t fdinfo_len;
};

//决定是否使用hashtable做struct event_io_map
#ifdef WIN32 
#define EVMAP_USE_HT 
#endif 

#ifdef EVMAP_USE_HT
#include "ht-internal.h"
struct event_map_entry;
HT_HEAD(event_io_map, event_map_entry);
#else
#define event_io_map event_signal_map
#endif

struct event_signal_map {
    /* An array of evmap_io * or of evmap_signal *; empty entries are
     * set to NULL. */
    void **entries;
    /* The number of entries available in entries */
    int nentries;
};

struct common_timeout_list {
	struct event_list events;
	struct timeval duration;
	struct event timeout_event;
	struct event_base *base;
};

/** Mask used to get the real tv_usec value from a common timeout. */
#define COMMON_TIMEOUT_MICROSECONDS_MASK       0x000fffff

struct event_change;
struct event_changelist {
	struct event_change *changes;
	int n_changes;
	int changes_size;
};

#ifndef _EVENT_DISABLE_DEBUG_MODE
/* Global internal flag: set to one if debug mode is on. */
extern int _event_debug_mode_on;
#define EVENT_DEBUG_MODE_IS_ON() (_event_debug_mode_on)
#else
#define EVENT_DEBUG_MODE_IS_ON() (0)
#endif //end  _EVENT_DISABLE_DEBUG_MODE

struct event_base {
	//Reactor初始化时采用的I/O复用机制
    const struct eventop *evsel;
	//指向I/O复用机制真正存储的数据，通过evsel成员的init函数来初始化
    void *evbase;
	//事件变化队列，当一个文件描述符上注册的事件被多次修改，则可以使用缓冲来避免重复的系统调用.
	//仅能用于 时间复杂度为O(1)的I/O复用技术(比如epoll_ctl)
    struct event_changelist changelist;
	//指向信号的后端处理机制
    const struct eventop *evsigsel;
	//信号事件处理器使用的数据结构，封装了一个由socketpair创建的管道，
	//用于信号处理函数和多路分发器之间的通信, 统一事件源
    struct evsig_info sig;
	//添加到本event_base的虚拟事件的数量
    int virtual_event_count;
	//添加到本event_base的所有事件的数量
    int event_count;
	//添加到本event_base的激活事件的数量
    int event_count_active;
	//是否执行完活动事件队列上剩余的任务之后就退出事件循环
    int event_gotterm;
	//是否立即退出事件循环，而不管是否还有任务需要处理
    int event_break;
	//是否立即启动一个新的事件循环
    int event_continue;
	//当前event_base正在处理的活动事件队列的优先级
    int event_running_priority;
	//事件循环是否已经启动
    int running_loop;
	//活动事件队列数组，索引值越小，优先级越高。高优先级的活动事件队列中的事件处理器优先处理
    struct event_list *activequeues;
	//活动事件队列数组的大小，即该event_base一共有多少个不同优先级的活动事件队列
    int nactivequeues;
	//通用定时器队列
    struct common_timeout_list **common_timeout_queues;
	//通用定时器队列中元素个数
    int n_common_timeouts;
	//通用定时器队列所占空间总大小
    int n_common_timeouts_allocated;
	//存放延迟回调函数的链表。事件循环每次成功处理完一个活动事件队列中的所有事件之后，就调用一次延迟回调函数
    struct deferred_cb_queue defer_queue;
	//文件描述符和I/O事件关系映射表
    struct event_io_map io;
	//信号值和信号事件之间的映射关系表
    struct event_signal_map sigmap;
	//注册时间队列，存放I/O事件处理器和信号事件处理器
    struct event_list eventqueue;
	//记录事件循环上一次检测的时间点
    struct timeval event_tv;
	//最小堆、时间堆
    struct min_heap timeheap;
	//存储时间点，避免太频繁调用gettimeofday/clock_gettime
    struct timeval tv_cache;

#if defined(_EVENT_HAVE_CLOCK_GETTIME) && defined(CLOCK_MONOTONIC)
	//clock_gettime和gettimeofday的差值
    struct timeval tv_clock_diff;
	//最后一次更新tv_clock_diff的时间
    time_t last_updated_clock_diff;
#endif

#ifndef _EVENT_DISABLE_THREAD_SUPPORT
	//运行当前事件循环的线程号
    unsigned long th_owner_id;
	//event_base独占锁
    void *th_base_lock;
	//当前事件循环正在执行哪个事件处理器的回调函数
    struct event *current_event;
	//条件变量，用于唤醒正在等待事件处理完毕的线程
    void *current_event_cond;
	//等待被唤醒的线程数
    int current_event_waiters;
#endif

#ifdef WIN32
    struct event_iocp_port *iocp;
#endif
	//event_base的一些配置参数
    enum event_base_config_flag flags;
	//event_base是否处于通知的未决状态。
	//即次线程已经通知了，但主线程还没处理这个通知
    int is_notify_pending;
	//唤醒主线程的双向管道
    evutil_socket_t th_notify_fd[2];
	//通知事件
    struct event th_notify;
	//其它线程唤醒主线程时调用的方法
    int (*th_notify_fn)(struct event_base *base);
};

struct event_config_entry {
	TAILQ_ENTRY(event_config_entry) next;

	const char *avoid_method;
};

struct event_config {
	TAILQ_HEAD(event_configq, event_config_entry) entries;

	int n_cpus_hint;
	enum event_method_feature require_features;
	enum event_base_config_flag flags;
};

#define N_ACTIVE_CALLBACKS(base)					\
	((base)->event_count_active + (base)->defer_queue.active_count)

int _evsig_set_handler(struct event_base *base, int evsignal,
			  void (*fn)(int));
int _evsig_restore_handler(struct event_base *base, int evsignal);

void event_active_nolock(struct event *ev, int res, short count);

#ifdef __cplusplus
}
#endif
#endif

