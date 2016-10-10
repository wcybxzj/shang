#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/select.h>

int max(int fd1, int fd2){
	if (fd1 > fd2) {
		return fd1;
	}else{
		return fd2;
	}
}

//测试:select 监视文件描述符号的数量
//ulimit -n 3000
//touch /tmp/{1..2000}

//./select_test 
//select 可以监控的文件描述符总数限制,FD_SETSIZE:1024
//count:1063
//maxfd:1066

//测试结果:实际能处理的数量不是1024而是1066
int main(int argc, const char *argv[])
{
	fd_set rset;
	int i, fd, num, maxfd, count=0;
	char str[20]={};
	printf("select 可以监控的文件描述符总数限制,FD_SETSIZE:%d\n", FD_SETSIZE);
	FD_ZERO(&rset);
	for (i = 1; i <= 2000; i++) {
		count++;
		snprintf(str,11,"/tmp/%d",i);
		fd = open(str, O_RDWR|O_CREAT);
		if (fd < 0) {
			perror("open():");
		}
		FD_SET(fd, &rset);
	}
	printf("count:%d\n", count);
	printf("maxfd:%d\n",fd+1);
	num = select(fd+1, &rset, NULL,NULL,NULL);
	if(num <0){
		perror("select():");
	}
	return 0;
}
