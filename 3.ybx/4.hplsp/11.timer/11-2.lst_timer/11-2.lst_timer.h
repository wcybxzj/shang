#ifndef LST_TIMER
#define LST_TIMER
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#define BUFFER_SIZE 64

struct client_data;

class util_timer
{
	public:
		time_t expire;
		void (*cb_func)(client_data*);
		client_data* user_data;
		util_timer* prev;
		util_timer* next;
		util_timer(): prev(NULL), next(NULL){}
};

//升序定时器链表
class sort_timer_lst
{
	private:
		util_timer* head;
		util_timer* tail;
		void add_timer(util_timer *timer, util_timer* lst_head);

	public:
		sort_timer_lst():head(NULL), tail(NULL){}
		~sort_timer_lst();
		void add_timer(util_timer* timer);
		void del_timer(util_timer* timer);
		void adjust_timer(util_timer* timer );
		void tick();
};

struct client_data
{
	sockaddr_in address;
	int sockfd;
	char buf[BUFFER_SIZE];
	util_timer* timer;
};

#endif
