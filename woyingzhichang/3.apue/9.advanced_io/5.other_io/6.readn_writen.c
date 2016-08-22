#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#define TEST_FILE "/tmp/test.txt"

#define BUF_SIZE 5

//坚持对fd读够n个字节
int readn(int fd, void *ptr, size_t n)
{
	int ret;
	int pos = 0;
	int len = n;
	while(len > 0){
		ret = read(fd, ptr+pos, len);
		if(ret < 0){
			if (errno == EINTR) {
				continue;
			}
			perror("read():");
			exit(1);
		}else if(ret == 0){//EOF:只能发生在读取普通文件的情况下
			//printf("%s\n","EOF");//debug
			break;
		}
		pos += ret;
		len -= ret;
	}
	return n - len;
}

int writen(int fd, void *ptr, size_t n){
	int ret;
	int pos = 0;
	int len = n;
	while(len > 0){
		ret = write(fd, ptr+pos, len);
		if(ret <= 0){
			if (errno == EINTR) {
				continue;
			}
			perror("read():");
			exit(1);
		}
		pos += ret;
		len -= ret;
	}
	return n - len;
}

void test_writen(void){
	int fd, num;
	//char *buf= "ybx";
	char *buf= "yangbingxi";
	fd = open(TEST_FILE, O_RDWR|O_CREAT|O_TRUNC);
	if(fd < 0){
		perror("open():");
		exit(1);
	}
	num = writen(fd, buf, strlen(buf));
	printf("writen is num:%d\n", num);
}

void test_file(void){
	int fd1,fd2, num1, num2;
	char buf[BUF_SIZE];
	fd1 = open("/etc/services", O_RDONLY);
	if(fd1 < 0){
		perror("open():");
		exit(1);
	}
	fd2 = open(TEST_FILE, O_WRONLY|O_CREAT|O_TRUNC|O_APPEND);

	while( (num1 = readn(fd1, buf, BUF_SIZE)) > 0 ){
		num2 = writen(fd2, buf, num1);
		if (num2 <= 0) {
			break;
		}
	}
	//printf("buf is :%s\n", buf);
	//printf("strlen buf is :%d\n", strlen(buf));
	//printf("readn num1 is :%d\n", num1);
}

int main(void){
	test_writen();
	test_file();
	exit(0);
}
