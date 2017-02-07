#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/select.h>
#include <errno.h>
#include <string.h>

//前提:
//ulimit -n 7000
//touch /tmp/{1..6000}

//测试1:证明打开6000个文件没问题
void test1()
{
	int i, fd, num, maxfd, count=0;
	char str[20]={};
	for (i = 1; i <= 6000; i++) {
		printf("11111111111 i:%d\n", i);
		count++;
		snprintf(str,11,"/tmp/%d",i);
		fd = open(str, O_RDWR|O_CREAT);
		if (fd < 0) {
			perror("open():");
			exit(1);
		}
		printf("2222222222 i:%d\n", i);
	}
	printf("cout:%d\n", count);
}

//测试2:本例证明select只能监控不超过open后值不超过1024的fd
void test2(int one_i, int two_i)
{
	int i, fd, num, maxfd, count=0;
	char str[20]={};
	//1.先打开一定数量的文件,目的增加当前open后fd的值
	for (i = 1; i <= one_i; i++) {
		snprintf(str,11,"/tmp/%d",i);
		fd = open(str, O_RDWR|O_CREAT);
		if (fd < 0) {
			perror("open():");
			exit(1);
		}
	}

	//2.open出一个很大的fd值,
	//本来select只监视1个fd, select应该返回1
	//如果open的fd超过1021很多select的工作就出问题 select就会返回很大一个数
	fd_set rset;
	FD_ZERO(&rset);
	snprintf(str,11,"/tmp/%d", two_i);
	fd = open(str, O_RDWR|O_CREAT);
	FD_SET(fd, &rset);

	printf("fd:%d\n", fd);
	printf("maxfd:%d\n", fd+1);
	num = select(fd+1, &rset, NULL,NULL,NULL);
	if(num <0){
		perror("select():");
	}else{
		printf("num:%d\n", num);
	}
}

//测试3:读fd_set/写fd_set/异常fd_set,每个fd_set只能设置1021个fd,比FD_SETSIZE还小
void test3(int max)
{
	fd_set rset;
	fd_set wset;
	fd_set errset;
	int i, fd, num, maxfd, count=0;
	char str[20]={};
	FD_ZERO(&rset);
	FD_ZERO(&wset);
	FD_ZERO(&errset);
	for (i = 1; i <= max; i++) {
		count++;
		snprintf(str,11,"/tmp/%d",i);
		fd = open(str, O_RDWR|O_CREAT);
		if (fd < 0) {
			perror("open():");
		}
		FD_SET(fd, &rset);
		FD_SET(fd, &wset);
		FD_SET(fd, &errset);
	}
	printf("fd:%d\n", fd);
	printf("count:%d\n", count);
	printf("maxfd:%d\n",fd+1);
	num = select(fd+1, &rset, &wset, &errset,NULL);
	if(num <0){
		perror("select():");
	}else{
		printf("num:%d\n", num);
	}
}

//测试4:select nfds的作用

void test4(int divisor)
{
	fd_set rset;
	int i, fd, num, maxfd;
	char str[20]={};
	FD_ZERO(&rset);
	for (i = 1; i <= 200; i++) {
		snprintf(str,11,"/tmp/%d",i);
		fd = open(str, O_RDWR|O_CREAT);
		if (fd < 0) {
			perror("open():");
		}
		FD_SET(fd, &rset);
	}
	printf("fd:%d\n", fd);
	printf("maxfd:%d\n",(fd+1)/divisor);


	num = select( (fd+1)/divisor, &rset, NULL , NULL ,NULL);
	if(num <0){
		perror("select():");
	}else{
		printf("num:%d\n", num);
	}
}


//测试结果:实际能处理的数量不是1024而是1066
int main(int argc, const char *argv[])
{
	//test1();

	/*
	./0.select_nfd 
	fd:1024
	maxfd:1025
	num:1 成功
	*/
	//test2(1021, 2001);

	/*
	./0.select_nfd 
	fd:1025
	maxfd:1026
	num:2 失败
	*/
	//test2(1022, 2001);

	/*
	./0.select_nfd 
	fd:1023
	count:1021
	maxfd:1024
	num:2042 (可读1021+可写1021) 正确
	*/
	//test3(1021);

	/*
	./0.select_nfd 
	fd:1024
	count:1022
	maxfd:1025
	num:2045  (可读1022+可写1022) 错误 应该2044
	*/
	//test3(1022);

	//FD_SET200个fd, select返回200
	//正常nfds的情况
	/*
	./0.select_nfd 
	fd:202
	maxfd:203
	num:200
	*/
	//test4(1);

	//FD_SET200个fd,select才返回98,
	//nfds除2的情况,有102fd没被select去监控
	/*
	./0.select_nfd 
	fd:202
	maxfd:101
	num:98
	*/
	test4(2);

	return 0;
}
