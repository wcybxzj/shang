#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <unistd.h>
#include<limits.h>
#include <signal.h>


//tlpi 63.2.3 其他备选的I/O设备
//表63-5
//
//POLLOUT: 不执行close(pd[0])
//
//POLLOUT|POLLERR: close(pd[0])
//
//POLLER 不方便模拟没模拟出
int main(int argc, const char *argv[])
{
	int pd[2];
	int ret, i;
	struct pollfd pollfd_var;

	ret = pipe(pd);
	if (ret==-1) {
		perror("pipe");
		exit(1);
	}

	//条件1:读是否关闭
	close(pd[0]);

	pollfd_var.fd = pd[1];
	pollfd_var.events = POLLOUT;

	ret = poll(&pollfd_var, 1, 0);
	if (ret == -1) {
		perror("poll");
		exit(1);
	}
	
	if (pollfd_var.revents & POLLERR) {
		printf("POLLERR\n");
	}
	if(pollfd_var.revents & POLLOUT){
		printf("POLLOUT\n");
	}
	if(pollfd_var.revents & POLLIN){
		printf("POLLIN\n");
	}
	return 0;
}
