#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/select.h>

//select在不同类型fd,不同事件的触发情况
//虚拟终端:
//读事件,只有对此虚拟终端输入内容才能触发
//写事件,只要能open就会触发
//异常事件,只有有异常才能触发

//普通文件:
//读事件,只要能open就会触发
//写事件,只要能open就会触发
//异常事件,只有有异常才能触发

int main(int argc, const char *argv[])
{
	int fd1;
	fd1 = open("/dev/tty11",O_CREAT|O_RDWR);
	//fd1 = open("/tmp/select.txt", O_CREAT|O_RDWR);
	if (fd1<0) {
		perror("open():");
		exit(1);
	}

	fd_set set;
	FD_ZERO(&set);
	FD_SET(fd1, &set);
	//3种事件
	//if (select(fd1+1, &set, NULL, NULL, NULL)<0) {
	if (select(fd1+1, NULL, &set, NULL, NULL)<0) {
	//if (select(fd1+1, NULL, NULL, &set, NULL)<0) {
		perror("select");
		exit(2);
	}
	printf("select is ok\n");

	return 0;
}
