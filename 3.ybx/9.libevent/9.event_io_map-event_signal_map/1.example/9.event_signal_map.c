#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <event3/util.h>
#include <event3/event.h>
#include "mm-internal.h" //此头本应是libvent内部使用 在这是测试下
#include "log-internal.h" //此头本应是libvent内部使用 在这是测试下
#include "event-internal.h"

#define MAX 10

//信号的回调函数
//event 是EV_SIGNAL
void my_fun(evutil_socket_t fd, short event, void* arg)
{
	printf("hell i am fd:%d\n",fd);
}

//1.event_struct.h  
//TAILQ_HEAD (event_list, event);
//struct event_list  
//{  
//    struct event *tqh_first;  
//        struct event **tqh_last;  
//        };  

//2.原本定义在evmap.c
struct evmap_signal {
	struct event_list events;
};

//3.event-internal.h中 struct event_signal_map 

//必须重新定义 原来的MY_GET_SIGNAL_SLOT在evmap.c中这里是无法使用的
#define MY_GET_SIGNAL_SLOT(x, map, slot, type)			\
		(x) = (struct type *)((map)->entries[slot])

//同上
#define MY_GET_SIGNAL_SLOT_AND_CTOR(x, map, slot, type, ctor)	\
	do {								\
		if ((map)->entries[slot] == NULL) {			\
			(map)->entries[slot] =				\
			    mm_calloc(1,sizeof(struct type)); \
			if (EVUTIL_UNLIKELY((map)->entries[slot] == NULL)) \
				return (-1);				\
			(ctor)((struct type *)(map)->entries[slot]);	\
		}							\
		(x) = (struct type *)((map)->entries[slot]);		\
	} while (0)


//come from evmap 原函数是static 所以需要在这里重新定义
static int
evmap_make_space(struct event_signal_map *map, int slot, int msize)
{
	if (map->nentries <= slot) {
		int nentries = map->nentries ? map->nentries : 32;
		void **tmp;

		while (nentries <= slot)
			nentries <<= 1;

		tmp = (void **)mm_realloc(map->entries, nentries * msize);
		if (tmp == NULL)
			return (-1);

		memset(&tmp[map->nentries], 0,
				(nentries - map->nentries) * msize);

		map->nentries = nentries;
		map->entries = tmp;
	}

	return (0);
}

//come from evmap 原函数是static 所以需要在这里重新定义
static void
evmap_signal_init(struct evmap_signal *entry)
{
	TAILQ_INIT(&entry->events);
}

//come fome event.c event_assign
int
my_event_assign(struct event *ev, evutil_socket_t fd,
			short events, void (*callback)(evutil_socket_t, short, void *arg),
			void *arg)
{
	ev->ev_callback = callback;
	ev->ev_arg = arg;
	ev->ev_fd = fd;
	ev->ev_events = events;
	ev->ev_res = 0;
	ev->ev_flags = EVLIST_INIT;
	return 0;
}

//come from event.c event_new
struct event *
my_event_new(evutil_socket_t fd, short events, \
	void (*cb)(evutil_socket_t, short, void*), void *arg)
{
	struct event *ev;
	ev = mm_malloc(sizeof(struct event));
	if (ev == NULL)
		return (NULL);
	if (my_event_assign(ev, fd, events, cb, arg) < 0) {
		mm_free(ev);
		return (NULL);
	}
	return (ev);
}

int  
my_evmap_signal_add(struct event_signal_map *map,  int sig, struct event *ev)  
{
	//4.evmap_make_space
	struct evmap_signal *ctx = NULL;
	if (sig >= map->nentries) {
		if (evmap_make_space(map, sig, sizeof(struct evmap_signal *)) == -1) {
			return -1;
		}
	}   
	//printf("%d", map->nentries);

	//5.
	//MY_GET_SIGNAL_SLOT_AND_CTOR(ctx, map, sig, evmap_signal, evmap_signal_init);
	do
	{
		//同event_io_map一样，同一个信号或者fd可以被多次event_new、event_add  
		//所以，当同一个信号或者fd被多次event_add后，entries[sig]就不会为NULL  
		if ((map)->entries[sig] == NULL)//第一次  
		{  
			//evmap_signal成员只有一个TAILQ_HEAD (event_list, event);  
			//可以说evmap_signal本身就是一个TAILQ_HEAD  
			//这个赋值操作很重要。  
			(map)->entries[sig] = mm_calloc(1, sizeof(struct evmap_signal));  

			if (EVUTIL_UNLIKELY((map)->entries[sig] == NULL)) {
				return -1;
			}

			//内部调用TAILQ_INIT(&entry->events);  
			(evmap_signal_init)((struct evmap_signal *)(map)->entries[sig]);  
		}  

		(ctx) = (struct evmap_signal *)((map)->entries[sig]);  
	} while (0);  

	//printf("%ld\n", ctx);
	//#define ev_signal_next	_ev.ev_signal.ev_signal_next
	TAILQ_INSERT_TAIL(&ctx->events, ev, ev_signal_next);

}


int main(int argc, const char *argv[])
{
	int sig;
	struct event* ev_arr[MAX];
	struct event_signal_map *map = mm_malloc(sizeof(struct event_signal_map));
	struct event *ev;
	struct evmap_signal *ctx;
	if (map==NULL) {
		event_err(1,"mm_malloc");
	}

	//同一个fd插入两次好看到尾队列的使用
	for (sig = 0; sig < MAX; sig++) {
		ev_arr[sig] = my_event_new(sig, 0, my_fun,(void *)sig);
		my_evmap_signal_add(map, sig, ev_arr[sig]);
	}
	printf("%d\n",map->nentries);
	for (sig = 0; sig < MAX; sig++) {
		ev_arr[sig] = my_event_new(sig, 0, my_fun,(void *)sig);
		my_evmap_signal_add(map, sig, ev_arr[sig]);
	}
	printf("%d\n",map->nentries);

	//循环下
	for (sig = 0; sig < MAX; sig++) {
		MY_GET_SIGNAL_SLOT(ctx, map, sig, evmap_signal);
		TAILQ_FOREACH(ev, &ctx->events , ev_signal_next) {
			printf("sig:%d\n", ev->ev_fd);
		}
	}


	return 0;
}
