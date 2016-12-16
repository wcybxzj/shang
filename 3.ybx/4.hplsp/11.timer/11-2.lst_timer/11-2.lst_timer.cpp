#include "11-2.lst_timer.h"

sort_timer_lst::~sort_timer_lst(){
	util_timer *tmp = head;
	while (tmp) {
		head = tmp->next;
		delete tmp;
		tmp = head;
	}
}

//情况1:1个元素都没有
//情况2:timer插在head前
//情况3:timer插在head后,timer不是最后一个元素
//情况4:timer插在最后一个元素后,timer是最后一个元素
//情况5:head->next为NULL,timer是最后一个元素
void sort_timer_lst::add_timer(util_timer* timer) {
	if (!timer) {
		return;
	}
	if (!head) {
		head = tail = timer;
		return;
	}
	if (timer->expire < head->expire) {
		timer->next = head;
		head->prev = timer;
		head = timer;
		return;
	}
	add_timer(timer, head);
}

void sort_timer_lst::del_timer(util_timer* timer) {
	if (!timer) {
		return;
	}
	if (timer == head && timer == tail) {
		delete timer;
		head = NULL;
		tail = NULL;
		return;
	}
	if (timer == head) {
		head = timer->next;
		head->prev = NULL;
		delete timer;
		return;
	}
	if (timer == tail) {
		tail = timer->prev;
		tail->next = NULL;
		delete timer;
		return;
	}
	timer->next->prev = timer->prev;
	timer->prev->next = timer->next;
	delete timer;
}

void sort_timer_lst::adjust_timer(util_timer* timer ) {
	if (!timer) {
		return;
	}
	util_timer* tmp = timer->next; 
	if (!tmp || (timer->expire < tmp->expire)) {
		return;
	}
	if (timer == head) {
		head = head->next;
		head->prev = NULL;
		timer->next = NULL;
		add_timer( timer, head);
	}else{
		timer->prev->next = timer->next;
		timer->next->prev = timer->prev;
		add_timer( timer, timer->next);
	}
}

void sort_timer_lst::tick() {
	if (!head) {
		return;
	}
	printf("tick work!\n");
	time_t cur = time(NULL);
	util_timer* tmp = head;

	while (tmp) {
		if (cur < tmp->expire) {
			break;
		}
		tmp->cb_func(tmp->user_data);
		head = tmp->next;
		if (head) {
			head->prev = NULL;
		}
		delete tmp;
		tmp = head;
	}
}

void sort_timer_lst::add_timer(util_timer *timer, util_timer* lst_head)
{
	util_timer* prev = lst_head;
	util_timer* tmp = prev->next;
	while (tmp) {
		if (timer->expire < tmp->expire) {
			prev->next = timer;
			tmp->prev = timer;
			timer->prev = prev;
			timer->next = tmp;
			return;
		}
		prev = tmp;
		tmp = tmp->next;
	}
	if (!tmp) {
		prev->next = timer;
		timer->prev = prev;
		timer->next = NULL;
		tail = timer;
	}
}
