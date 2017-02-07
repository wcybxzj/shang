#include <stdio.h>

//include/event2/event_struct.h
#define EVLIST_TIMEOUT	0x01
#define EVLIST_INSERTED	0x02
#define EVLIST_SIGNAL	0x04
#define EVLIST_ACTIVE	0x08
#define EVLIST_INTERNAL	0x10
#define EVLIST_INIT	0x80 //128
/* EVLIST_X_ Private space: 0x1000-0xf000 */
#define EVLIST_ALL	(0xf000 | 0x9f)

//event.c-->event_queue_remove()
int main(int argc, const char *argv[])
{
	short ev_flags;

	//测试1:证明取反先运行
	ev_flags = EVLIST_INTERNAL|EVLIST_TIMEOUT;
	printf("%d\n", ~ev_flags & EVLIST_INTERNAL);//0
	printf("%d\n", (~ev_flags) & EVLIST_INTERNAL);//0
	//h is short,u is unsigned
	printf("%hu\n", ~(ev_flags & EVLIST_INTERNAL));//65519-->1111111111101111

	//测试2:
	ev_flags =EVLIST_TIMEOUT|EVLIST_INSERTED;
	//写法1:
	if (~ev_flags & EVLIST_INTERNAL){
		printf("说明ev_flags不含有EVLIST_INTERNAL\n");
	}
	//写法2:
	if (!(ev_flags & EVLIST_INTERNAL)){
		printf("说明ev_flags不含有EVLIST_INTERNAL\n");
	}

	return 0;
}
