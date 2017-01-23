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
	int i;
	FILE *fp;
	char buf[SIZE]={0,};
	fp = fopen("/etc/services", "r");
	for (i = 0; i < 10; i++) {
		fgets(buf, SIZE, fp);
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
	FILE *fp;
	int i;
	char buf[SIZE];
	fp = fopen("/tmp/123.txt", "a");
	if (fp==NULL) {
		perror("fopen");
		exit(1);
	}
	for (i = 0; i < 100; i++) {
		sprintf(buf,"pid:%d,i:%d\n", getpid(), i);
		fputs(buf, fp);
	}
}

//测试3:
//无法完成工作，父进程工作正常，子进程不正常 一下读到finger那行
void fork_read()
{
	//10.question/1.io_parallel/2.stdio/1.study_stdio/1.fork.c
	//-->test_stdio_inner_struct
}

//测试4:父子进程共同写一个文件
//应该和测试2结果一样不测了
void fork_write()
{

}
