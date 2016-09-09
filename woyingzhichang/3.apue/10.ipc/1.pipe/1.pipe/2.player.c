#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SIZE 1024

#define NAME "3.mp3"
int main(){
	pid_t pid;
	int len,fd;
	char buf[SIZE];
	int pd[2];
	if(pipe(pd) < 0){
		perror("perror():");
		exit(0);
	}

	pid = fork();
	if(pid == 0){
		close(pd[1]);
		dup2(pd[0], 0);
		close(pd[0]);
		fd = open("/dev/null", O_RDWR);
		//fd = open("/tmp/1.txt", O_RDWR|O_CREAT|O_TRUNC);
		dup2(fd,1);
		dup2(fd,2);
		execl("/usr/local/bin/mpg123", "mpg123", "-", NULL);
		//execl("/root/www/shangguan/woyingzhichang/3.apue/10.ipc/1.pipe/4.mympg123",\
				"4.mympg123", "-", NULL);
		
		perror("execl():");
		exit(0);
	}else if(pid >0){
		close(pd[0]);
		fd = open(NAME, O_RDONLY);
		if (fd < 0) {
			perror("open():");
			exit(-1);
		}
		while( (len = read(fd, buf, SIZE)) >0){
			write(pd[1], buf, len);
		}
		close(fd);
		wait(NULL);
	}
	exit(0);
}
