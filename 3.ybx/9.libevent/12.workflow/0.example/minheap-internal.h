#ifndef _MIN_HEAP_H_
#define _MIN_HEAP_H_

#include <event3/event-config.h>
#include <event3/event.h>
#include <event3/event_struct.h>
#include <event3/util.h>
#include "util-internal.h"
#include "mm-internal.h"

 //struct event只关注:
 //min_heap_idx：表示该event保存在min_heap数组中的索引，初始为-1；
 //ev_timeout：该event的超时时间，将被用于heap操作中的节点值比较。
typedef struct min_heap
{
	struct event** p;//0下标是第一个元素
	unsigned n, a;//n队列元素的多少,a代表队列空间的大小
} min_heap_t;

static inline void	     min_heap_ctor(min_heap_t* s);
static inline void	     min_heap_dtor(min_heap_t* s);
static inline void	     min_heap_elem_init(struct event* e);
static inline int	     min_heap_elt_is_top(const struct event *e);
static inline int	     min_heap_elem_greater(struct event *a, struct event *b);
static inline int	     min_heap_empty(min_heap_t* s);
static inline unsigned	     min_heap_size(min_heap_t* s);
static inline struct event*  min_heap_top(min_heap_t* s);
static inline int	     min_heap_reserve(min_heap_t* s, unsigned n);
static inline int	     min_heap_push(min_heap_t* s, struct event* e);
static inline struct event*  min_heap_pop(min_heap_t* s);
static inline int	     min_heap_erase(min_heap_t* s, struct event* e);
static inline void	     min_heap_shift_up_(min_heap_t* s, unsigned hole_index, struct event* e);
static inline void	     min_heap_shift_down_(min_heap_t* s, unsigned hole_index, struct event* e);

int min_heap_elem_greater(struct event *a, struct event *b)
{
	return evutil_timercmp(&a->ev_timeout, &b->ev_timeout, >);
}

void min_heap_ctor(min_heap_t* s) { s->p = 0; s->n = 0; s->a = 0; }
void min_heap_dtor(min_heap_t* s) { if (s->p) mm_free(s->p); }
void min_heap_elem_init(struct event* e) { e->ev_timeout_pos.min_heap_idx = -1; }
int min_heap_empty(min_heap_t* s) { return 0u == s->n; }
unsigned min_heap_size(min_heap_t* s) { return s->n; }
struct event* min_heap_top(min_heap_t* s) { return s->n ? *s->p : 0; }

int min_heap_push(min_heap_t* s, struct event* e)
{
	if (min_heap_reserve(s, s->n + 1))
		return -1;
	min_heap_shift_up_(s, s->n++, e);
	return 0;
}

struct event* min_heap_pop(min_heap_t* s)
{
	if (s->n)
	{
		struct event* e = *s->p;
		min_heap_shift_down_(s, 0u, s->p[--s->n]);
		e->ev_timeout_pos.min_heap_idx = -1;
		return e;
	}
	return 0;
}

int min_heap_elt_is_top(const struct event *e)
{
	return e->ev_timeout_pos.min_heap_idx == 0;
}

//移除元素并保持堆性质不变
int min_heap_erase(min_heap_t* s, struct event* e)
{
	if (-1 != e->ev_timeout_pos.min_heap_idx)
	{
		struct event *last = s->p[--s->n];
		unsigned parent = (e->ev_timeout_pos.min_heap_idx - 1) / 2;

		//我们用在堆中最后一个元素替换e。
		//e的位置大于0并且last小于e的parent,则向上
		//e的位置大于0并且last大于等于e的parent,则向下
		//e的位置等于0实际和min_heap_pop一样,则向下
		/* we replace e with the last element in the heap.  We might need to
		   shift it upward if it is less than its parent, or downward if it is
		   greater than one or both its children. Since the children are known
		   to be less than the parent, it can't need to shift both up and
		   down. */
		if (e->ev_timeout_pos.min_heap_idx > 0 && min_heap_elem_greater(s->p[parent], last))
			min_heap_shift_up_(s, e->ev_timeout_pos.min_heap_idx, last);
		else
			min_heap_shift_down_(s, e->ev_timeout_pos.min_heap_idx, last);
		e->ev_timeout_pos.min_heap_idx = -1;
		return 0;
	}
	return -1;
}

//分配队列大小.n代表队列元素个数多少.
int min_heap_reserve(min_heap_t* s, unsigned n)
{   
	//队列大小不足元素个数,重新分配空间.
	if (s->a < n)
	{
		struct event** p;
		//初始分配8个指针大小空间,否则原空间大小翻倍.
		unsigned a = s->a ? s->a * 2 : 8;
		//翻倍后空间依旧不足,则分配n.
		if (a < n)
			a = n;
		//重新分配内存
		if (!(p = (struct event**)mm_realloc(s->p, a * sizeof *p)))
			return -1;
		s->p = p; //重新赋值队列地址及大小.
		s->a = a; //
	}
	return 0;
}

void min_heap_shift_up_(min_heap_t* s, unsigned hole_index, struct event* e)
{
    unsigned parent = (hole_index - 1) / 2;
    while (hole_index && min_heap_elem_greater(s->p[parent], e))
    {
		(s->p[hole_index] = s->p[parent])->ev_timeout_pos.min_heap_idx = hole_index;
		hole_index = parent;
		parent = (hole_index - 1) / 2;
	}
    (s->p[hole_index] = e)->ev_timeout_pos.min_heap_idx = hole_index;
}

//1.说明:
//gaouifan和zhangming的思路:
//弹出要出堆元素，然后从这个元素位置进行堆化
//libevent的思路:
//弹出要出堆元素，
//如果要出堆元素所有孩子都在最小堆范围内,优先让孩子之一来填充自己
//否则让最后一个元素来填充自己
//2.第三个参数e指向的是最小堆中最后一个元素
void min_heap_shift_down_(min_heap_t* s, unsigned hole_index, struct event* e)
{
	/* 取得hole_index的右孩子节点索引 */
    unsigned min_child = 2 * (hole_index + 1);
	//printf("min_child:%d\n", min_child);
	//printf("s->n:%d\n", s->n);

    while (min_child <= s->n)
	{
		//min_child -= ( min_child == s->n || min_heap_elem_greater(s->p[min_child], s->p[min_child - 1]) );
		min_child -= min_child == s->n || min_heap_elem_greater(s->p[min_child], s->p[min_child - 1]);
		if (!(min_heap_elem_greater(e, s->p[min_child])))
		    break;
		(s->p[hole_index] = s->p[min_child])->ev_timeout_pos.min_heap_idx = hole_index;
		hole_index = min_child;
		min_child = 2 * (hole_index + 1);
	}
    (s->p[hole_index] = e)->ev_timeout_pos.min_heap_idx = hole_index;
}



#endif //end of  _MIN_HEAP_H_
