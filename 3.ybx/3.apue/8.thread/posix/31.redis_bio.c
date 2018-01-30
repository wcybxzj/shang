#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define FILE "/tmp/123"
int fd=0;

static char *genstr(char *prefix, int i) {
    static char result[64] = {0};
    snprintf(result, sizeof(result), "%s%d\n", prefix, i);
    return result;
}

static void* func(void*ptr)
{
	int i;
	sleep(3);
	if (fd>0) {
		printf("子线程close fd!!!\n");
		close(fd);
	}
	pthread_exit(0);
}

//redis bio有两种类型
//1个是异步关闭文件(本次测试这个)
//1个是异步fdatasync

//这里测试主线程打开的文件,让子线程关闭后 主线程就无法再进行操作
//再次说明多线程文件描述符数组是共享的
/*
主线程:working on thread_one:0
主线程:write ok
主线程:working on thread_one:1
主线程:write ok
子线程close fd!!!
主线程:working on thread_one:2
主线程:write error:Bad file descriptor
*/
int main(int argc, const char *argv[])
{
	int err;
	int i,ret;
	char *str;
	fd = open(FILE,O_CREAT|O_TRUNC|O_RDWR);

	pthread_t tid;
	err = pthread_create(&tid, NULL, func, NULL );
	if (err) {
		fprintf(stderr, "%s\n", strerror(err));
		exit(1);
	}

	for (i = 0; i < 100; i++) {
		sleep(1);
		str = genstr("thread_one:",i);
		ret = write(fd,str, strlen(str));
		printf("主线程:working on %s",str);
		if (ret ==-1) {
			printf("主线程:write error:%s\n",strerror(errno));
			exit(1);
		}else{
			printf("主线程:write ok\n");
		}
	}

	return 0;
}
