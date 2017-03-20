#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define NUM 10

struct event {
	struct timeval ev_timeout;
};

typedef struct min_heap
{
	struct event** p;//0下标是第一个元素
	unsigned n, a;//n队列元素的多少,a代表队列空间的大小
} min_heap_t;

struct event_base {
	struct min_heap timeheap;
};

void add(struct timeval *tvp)
{
	tvp->tv_sec++;
}

void time_correct(struct event_base *base)
{
	int i, n;
	struct event** p;
	p = base->timeheap.p;
	n = base->timeheap.n;
	//主要测试一下这
	for (; n-->0; ++p) {
		//printf("%d\n",(**p).ev_timeout.tv_sec );
		struct timeval *ev_tv = &(**p).ev_timeout;
		//struct timeval *ev_tv = &((**p).ev_timeout);
		add(ev_tv);
	}
}

int main(int argc, const char *argv[])
{
	int i;
	struct event_base base;
	base.timeheap.p = calloc(NUM+1, sizeof(struct event*));
	base.timeheap.n = 0;

	for (i = 0; i < NUM; i++) {
		base.timeheap.p[i]= calloc(1, sizeof (struct event));
		base.timeheap.p[i]->ev_timeout.tv_sec=i;
		base.timeheap.n++;
	}

	for (i = 0; i < NUM; i++) {
		printf("%d\n", base.timeheap.p[i]->ev_timeout.tv_sec);
	}
	printf("=============================================\n");

	time_correct(&base);

	for (i = 0; i < NUM; i++) {
		printf("%d\n", base.timeheap.p[i]->ev_timeout.tv_sec);
	}

	return 0;
}
