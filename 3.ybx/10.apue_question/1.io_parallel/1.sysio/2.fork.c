#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#define SIZE 30
#define FNAME "/tmp/123"

void normal_read();
void normal_write();

int main(int argc, char *argv[])
{
	//normal_read();
	normal_write();
	return 0;
}

//测试1:两个无关进程不受影响,各自读取全部内容读取正常
//终端1:./2.fork
//终端2:./2.fork
void normal_read()
{
	int i, fd;
	char buf[SIZE]={0,};
	fd = open(FNAME, O_RDONLY);
	for (i = 0; i < 10; i++) {
		read(fd, buf, SIZE-1);
		printf("%s", buf);
		sleep(1);
	}
}

//测试2:100个无关进程不受影响,写正常
//rm /tmp/123.txt
//./2.sh
//vim /tmp/123.txt 正好是1万行
void normal_write()
{
	int i, fd;
	char buf[SIZE]={0,};
	fd = open("/tmp/123.txt", O_RDWR|O_CREAT|O_APPEND);
	for (i = 0; i < 100; i++) {
		sprintf(buf,"pid:%d, time:%ld, i:%d\n",
				getpid(), time(NULL), i);
		write(fd, buf, strlen(buf));
	}
}


//测试4:父子进程共同写一个文件
//应该和测试2结果一样不测了
void fork_write()
{

}

//测试3:
//父子进程读取同一个文件的情况,两个进程协作完成任务
//父子进程分别交替读取同一个文件的不同部分
void fork_read()
{
	//3.apue/2.sysio/5.study_sysio/1.fork.c-->test_sysio_inner_struct()
}
