#define _GNU_SOURCE     /* To get definitions of 'OFD' locking commands */
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <strings.h>

#define FNAME "./1.txt"

#define SIZE 1024

//知识点1:
//l_start:0是其实位置

//知识点2:
//这里是说要在此范围设置一把写锁看是否冲突
//如果冲突返回冲突锁的属性

//目的:测试一个进程上写锁，另外一个进程判断是否可以上锁
void func1()
{
	int pid, fd, cmd, status;
	char buf[SIZE]="abcdefghijklmnopqrstuvwxyz";
	struct flock fl;

	fd = open(FNAME, O_RDWR|O_CREAT|O_TRUNC, 0666);
	if (fd ==-1) {
		perror("open");
		exit(1);
	}

	write(fd, buf, SIZE);

	//上一个写锁
	fl.l_start = 3;
	fl.l_len = 20;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
	fl.l_pid = 0; 
	cmd = F_SETLK;
    status = fcntl(fd, cmd, &fl);           
	if (status==-1) {
		perror("fcntl");
		exit(1);
	}
	pid = fork();

	//其他进程测试是否可以上锁
	if (pid==0) {
		bzero(&fl, sizeof(fl));
		printf("child pid:%d\n", getpid());
		sleep(1);
		fl.l_start = 3;
		fl.l_len = 20;
		fl.l_type = F_WRLCK;
		fl.l_whence = SEEK_SET;
		fl.l_pid = 0; 
		cmd = F_GETLK;//检测锁是否冲突
		status = fcntl(fd, cmd, &fl);           
		if (status==-1) {
			perror("fcntl");
			exit(1);
		}

		if (fl.l_type==F_UNLCK) {
			printf("锁不存在冲突,可以上锁\n");
		}else{
			printf("锁存在冲突,不可以上锁\n");
			if (fl.l_type==F_WRLCK) {
				printf("锁类型为:%s\n","写锁");
			}
		}
		exit(1);
	}

	printf("parent pid:%d\n", getpid());
	wait(NULL);
	exit(0);
}

//目的:
//在一个进程中用一个写锁分割一把读锁,形成3把锁
//另外一个进程锁测试
//这个测试实际是tlpi图55-3的代码
void func2()
{
	int pid, fd, cmd, status;
	char buf[SIZE]="abcdefghijklmnopqrstuvwxyz";
	struct flock fl;

	fd = open(FNAME, O_RDWR|O_CREAT|O_TRUNC, 0666);
	if (fd ==-1) {
		perror("open");
		exit(1);
	}

	write(fd, buf, SIZE);

	//上一个读锁,3-22
	fl.l_start = 3;
	fl.l_len = 20;
	fl.l_type = F_RDLCK;
	fl.l_whence = SEEK_SET;
	fl.l_pid = 0; 
	cmd = F_SETLK;
	status = fcntl(fd, cmd, &fl);           
	if (status==-1) {
		perror("fcntl");
		exit(1);
	}else{
		printf("上读锁成功\n");
	}

	//在读锁中间上一把写锁
	//读锁:3-12
	//写锁:13-17
	//读锁:18-22
	fl.l_start = 13;
	fl.l_len = 5;
	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_pid = 0; 
	cmd = F_SETLK;
	status = fcntl(fd, cmd, &fl);           
	if (status==-1) {
		perror("fcntl");
		exit(1);
	}else{
		printf("上写锁锁成功\n");
	}

	pid = fork();
	if (pid==0) {
		sleep(1);

		//测试文件,3-12字节的读锁
		bzero(&fl, sizeof(fl));
		fl.l_start = 3;
		fl.l_len = 10;
		fl.l_type = F_WRLCK;
		fl.l_whence = SEEK_SET;
		fl.l_pid = 0; 
		cmd = F_GETLK;//检测锁是否冲突
		status = fcntl(fd, cmd, &fl);           
		if (status==-1) {
			perror("fcntl");
			exit(1);
		}
		if (fl.l_type==F_UNLCK) {
			printf("锁不存在冲突,可以上锁\n");
		}else{
			printf("锁存在冲突,不可以上锁\n");
			printf("冲突锁信息:\n");
			printf("l_start:%d\n", fl.l_start);
			printf("l_len:%d\n", fl.l_len);
			if (fl.l_type==F_WRLCK) {
				printf("锁类型为:%s\n","写锁");
			}else if(fl.l_type==F_RDLCK){
				printf("锁类型为:%s\n","读锁");
			}
		}

		//测试文件,13-17写锁
		bzero(&fl, sizeof(fl));
		fl.l_start = 13;
		fl.l_len = 5;
		fl.l_type = F_RDLCK;
		fl.l_whence = SEEK_SET;
		fl.l_pid = 0; 
		cmd = F_GETLK;//检测锁是否冲突
		status = fcntl(fd, cmd, &fl);           
		if (status==-1) {
			perror("fcntl");
			exit(1);
		}
		if (fl.l_type==F_UNLCK) {
			printf("锁不存在冲突,可以上锁\n");
		}else{
			printf("锁存在冲突,不可以上锁\n");
			printf("冲突锁信息:\n");
			printf("l_start:%d\n", fl.l_start);
			printf("l_len:%d\n", fl.l_len);
			if (fl.l_type==F_WRLCK) {
				printf("锁类型为:%s\n","写锁");
			}else if(fl.l_type==F_RDLCK){
				printf("锁类型为:%s\n","读锁");
			}
		}

		//测试文件,18-22读锁
		bzero(&fl, sizeof(fl));
		fl.l_start = 18;
		fl.l_len = 5;
		fl.l_type = F_WRLCK;
		fl.l_whence = SEEK_SET;
		fl.l_pid = 0; 
		cmd = F_GETLK;//检测锁是否冲突
		status = fcntl(fd, cmd, &fl);           
		if (status==-1) {
			perror("fcntl");
			exit(1);
		}
		if (fl.l_type==F_UNLCK) {
			printf("锁不存在冲突,可以上锁\n");
		}else{
			printf("锁存在冲突,不可以上锁\n");
			printf("冲突锁信息:\n");
			printf("l_start:%d\n", fl.l_start);
			printf("l_len:%d\n", fl.l_len);
			if (fl.l_type==F_WRLCK) {
				printf("锁类型为:%s\n","写锁");
			}else if(fl.l_type==F_RDLCK){
				printf("锁类型为:%s\n","读锁");
			}
		}



		exit(0);//child done
	}

	wait(NULL);
}

int main(int argc, const char *argv[])
{
	//func1();
	func2();
	return 0;
}
