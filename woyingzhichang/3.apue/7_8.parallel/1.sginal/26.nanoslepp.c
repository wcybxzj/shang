#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

void sig_handler(int s)
{

}
//按住Ctrl+c 看效果坚持睡够20秒
int main(int argc, const char *argv[])
{
	signal(SIGINT, sig_handler);
	struct timespec origin_ts, left_ts;
	origin_ts.tv_sec = 20;

	while (nanosleep(&origin_ts, &left_ts)<0) {
		if (errno==EINTR) {
			printf("caught signal\n");
			printf("left time is sec:%ld nsec:%ld\n", left_ts.tv_sec, left_ts.tv_nsec);
			origin_ts = left_ts;
			continue;
		}
		perror("nanosleep():");
	}
	return 0;
}
