#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define SIZE 10
#define FNAME "/etc/services"

void test_stdio_inner_struct();
void test_stdio_FILE_star();

int main(int argc, const char *argv[])
{
	test_stdio_inner_struct();
	//test_stdio_FILE_star();
	return 0;
}

//无法完成工作，父进程工作正常，子进程不正常 一下读到finger那行
//./1.fork 
//>>parent pid:6769, buf:# /etc/services:
//>>child pid:6770, buf:ervice
//<<>>parent pid:6769, buf:# $Id: services,v 1.48 2009/11/11 14:32:31 ovasik Exp $
//<<>>child pid:6770, buf:finger          79/tcp
//<<>>parent pid:6769, buf:#
//<<>>child pid:6770, buf:finger          79/udp
//<<>>child pid:6770, buf:http            80/tcp          www www-http    # WorldWideWeb HTTP
void test_stdio_inner_struct()
{
	pid_t pid;
	FILE* fp;
	char buf[SIZE]={0,};
	fp = fopen(FNAME, "r");
	int i;

	pid =  fork();
	if (pid==0) {
		for (i = 0; i < 10; i++) {
			fgets(buf, SIZE, fp);
			printf(">>child pid:%d, buf:%s<<", getpid(), buf);
			sleep(1);
		}
		fclose(fp);
		exit(0);
	}
	for (i = 0; i < 10; i++) {
		fgets(buf, SIZE, fp);
		printf(">>parent pid:%d, buf:%s<<", getpid(), buf);
		sleep(1);
	}
	wait(NULL);
	fclose(fp);
}

/*
什么也不说明, 只要连续调用fopen就是返回的堆空间就是0x40一增长
./1.fork 
parent pid:7202, fp:7e3010
parent pid:7202, fp:7e3250
parent pid:7202, fp:7e3490
child pid:7203, fp:7e3010
child pid:7203, fp:7e3250
child pid:7203, fp:7e3490
*/
void test_stdio_FILE_star()
{
	FILE *fp;
    int i=0;
    pid_t pid;
    pid = fork();
    if (pid == 0) {
        while (1) {
            fp = fopen(FNAME, "r");
            printf("child pid:%d, fp:%x\n", getpid(), fp);  
            i++;            
			if (i%3==0) {
                sleep(1);
            }       
        }       
        exit(0);
    }       
    while (1) {
        fp = fopen(FNAME, "r");
        printf("parent pid:%d, fp:%x\n", getpid(), fp);
        i++;
        if (i%3==0) {
            sleep(5);
        }
    }
    wait(NULL);
}
