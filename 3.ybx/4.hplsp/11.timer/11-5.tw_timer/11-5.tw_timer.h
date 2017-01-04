#ifndef TIME_WHEEL_TIMER
#define TIME_WHEEL_TIMER

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#define BUFFER_SIZE 64

class tw_timer;

struct client_data
{
	sockaddr_in address;
	int sockfd;
	char buf[BUFFER_SIZE];
	tw_timer* timer;
};

class tw_timer
{
	public:
		tw_timer(int rot, int ts):
		next(NULL), prev(NULL), rotation(rot), time_slot(ts){
			printf("new tw_timer rotation:%d\n", rotation);
			printf("new tw_timer time_slot:%d\n", time_slot);
		}

		int rotation;
		int time_slot;
		void (*cb_func)(client_data *);
		client_data* user_data; 
		tw_timer* next;
		tw_timer* prev;
};

class time_wheel{
	private:
		static const int N = 60;//1个时间轮分多少slot
		static const int TI = 1;//每次tick的间隔时间
		tw_timer* slots[N];
		int cur_slot;

	public:
		time_wheel(): cur_slot(0){ 
			int i;
			for (i = 0; i < N; i++) {
				slots[i] = NULL;
			}
		}

		~time_wheel()
		{
			int i;
			for (i = 0; i < N ; i++) {
				tw_timer * tmp = slots[i];
				while (tmp) {
					slots[i] = tmp->next;
					delete tmp;
					tmp = slots[i];
				}
			}
		}

		//例:
		//timeout:是3秒
		//TI:每次tick的时间 假设1秒
		//ticks:传入的timeout转化成多少次ticks
		//结果:ticks = 3/1
		//rotation:圈数
		tw_timer *add_timer(int timeout)
		{
			if (timeout < 0) {
				return NULL;
			}
			int ticks = 0;
			if (timeout< TI) {
				ticks = 1;
			}else{
				ticks = timeout / TI;
			}

			int rotation = ticks / N;
			int ts = (cur_slot + (ticks % N)) % N;
			tw_timer* timer = new tw_timer(rotation, ts);
			if (!slots[ts]) {
				slots[ts] = timer;
			}else{
				timer->next = slots[ts];
				slots[ts]->prev = timer;
				slots[ts] = timer;
			}
			printf("add_timer ticks:%d  rotation:%d\n", ticks, rotation);
			return timer;
		}

		void del_timer(tw_timer* timer)
		{
			if (!timer) {
				return;
			}
			int ts = timer->time_slot;
			if (timer == slots[ts]) {//timer 是链表头
				slots[ts] = slots[ts]->next;
				if (slots[ts]) {
					slots[ts]->prev = NULL;
				}
				delete timer;
			}else{
				timer->prev->next = timer->next;
				if (timer->next) {
					timer->next->prev = timer->prev;
				}
				delete timer;
			}
		}

		//每次滴答执行,时间轮指定slot,在圈数为0的timer->cb_func
		void tick()
		{
			tw_timer* tmp = slots[cur_slot];
			printf("current slot is:%d\n", cur_slot);
			while (tmp) {
				if (tmp->rotation > 0) {
					tmp->rotation--;
					tmp = tmp->next;
				}else{
					tmp->cb_func(tmp->user_data);
					if (tmp==slots[cur_slot]) {
						slots[cur_slot] = tmp->next;
						delete tmp;
						if (tmp->next) {
							slots[cur_slot]->prev = NULL;
						}
						tmp = slots[cur_slot];
					}else{
						tmp->prev->next = tmp->next;
						if (tmp->next) {
							tmp->next->prev = tmp->prev;
						}
						tw_timer *tmp2 = tmp->next;
						delete tmp;
						tmp = tmp2;
					}
				}
			}
			cur_slot++;
			cur_slot = cur_slot%N;
		}

		void adjust_timer(tw_timer* timer)
		{
			if (!timer) {
				return;
			}
			timer->rotation++;
		}
};

#endif

