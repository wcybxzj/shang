#include <stdio.h>
#include <assert.h>

//include/event2/event_struct.h
#define EVLIST_TIMEOUT	0x01
#define EVLIST_INSERTED	0x02
#define EVLIST_SIGNAL	0x04
#define EVLIST_ACTIVE	0x08
#define EVLIST_INTERNAL	0x10
#define EVLIST_INIT	0x80 //128
/* EVLIST_X_ Private space: 0x1000-0xf000 */
#define EVLIST_ALL	(0xf000 | 0x9f)

//event.c --> event_del_internal() -->EVUTIL_ASSERT(!(ev->ev_flags & ~EVLIST_ALL));
//判断ev_flags是否是 EVLIST_XXX 或出来的,
//如果是EVUTIL_ASSERT预期结果为0就不会报错否则报错
int main(int argc, const char *argv[])
{
	//61599->1111000010011111
	//3936 ->	 111101100000
	printf("%d\n", EVLIST_ALL);//61599
	printf("%hd\n", ~EVLIST_ALL);//3936

	short ev_flags = EVLIST_TIMEOUT;
	printf("%d\n", ev_flags & ~EVLIST_ALL);//0
	assert(!(ev_flags & ~EVLIST_ALL));//不会报错

	ev_flags = EVLIST_TIMEOUT |EVLIST_INSERTED |EVLIST_SIGNAL |EVLIST_ACTIVE |EVLIST_INTERNAL|EVLIST_INIT;
	printf("%d\n", ev_flags & ~EVLIST_ALL);//0
	assert(!(ev_flags & ~EVLIST_ALL));//不会报错

	return 0;
}
