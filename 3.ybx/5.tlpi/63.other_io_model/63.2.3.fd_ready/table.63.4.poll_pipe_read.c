#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <unistd.h>
//tlpi 63.2.3 其他备选的I/O设备
//表63-4
//POLLHUP:		 close(pd[1])并且pipe没数据
//POLLIN:		 注释close(pd[1])并且pipe有数据
//POLLIN|POLLHUP:close(pd[1])并且pipe有数据
void func1()
{
	int pd[2];
	int ret;
	struct pollfd pollfd_var;
	ret = pipe(pd);
	if (ret==-1) {
		perror("pipe");
		exit(1);
	}

	//
	write(pd[1],"abc",4);

	//
	close(pd[1]);

	pollfd_var.fd = pd[0];
	pollfd_var.events = POLLIN|POLLOUT;

	ret = poll(&pollfd_var, 1, 0);
	if (ret == -1) {
		perror("poll");
		exit(1);
	}
	
	if (pollfd_var.revents & POLLHUP) {
		printf("POLLHUP\n");
	}
	if(pollfd_var.revents & POLLIN){
		printf("POLLIN\n");
	}
	if(pollfd_var.revents & POLLOUT){
		printf("POLLOUT\n");
	}

}


int main(int argc, const char *argv[])
{
	func1();
}
