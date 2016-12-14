#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include "lst_timer.h"
#include <iostream>
#include <cstring>
using namespace std;

sort_timer_lst *lst_p = NULL;
static void alrm_handler(int s, siginfo_t * info, void *ucp)
{
	lst_p->tick();
}

void myfunc(client_data *pt)
{
	cout<<pt->buf<<endl;
}

int main(int argc, const char *argv[])
{
	client_data* data_pt = new client_data;
	if (data_pt == NULL) {
		perror("new()");
		exit(1);
	}
	strcpy(data_pt->buf, "this is one");
	util_timer *timer_p1 = new util_timer(time(NULL)+5, myfunc, data_pt);

	client_data* data_pt2 = new client_data;
	if (data_pt2 == NULL) {
		perror("new()");
		exit(1);
	}
	strcpy(data_pt2->buf, "this is two");
	util_timer *timer_p2 = new util_timer(time(NULL)+10, myfunc, data_pt2);

	lst_p = new sort_timer_lst;
	lst_p->add_timer(timer_p1);
	lst_p->add_timer(timer_p2);

	struct sigaction sa;
	struct itimerval itv;

	sa.sa_sigaction=alrm_handler;
	sa.sa_flags=0;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGALRM, &sa, NULL) < 0){
		perror("sigaction");
		exit(1);
	}

	//初始值1秒，成为0后发信号给进程
	//并且用it_interval传递到it_value
	itv.it_value.tv_sec = 1;
	itv.it_value.tv_usec = 0;
	itv.it_interval.tv_sec = 1;
	itv.it_interval.tv_usec = 0;

	if (setitimer(ITIMER_REAL, &itv, NULL) < 0) {
		perror("setitimer");
		exit(1);
	}

	while (1) {
		pause();
	}
	return 0;
}
