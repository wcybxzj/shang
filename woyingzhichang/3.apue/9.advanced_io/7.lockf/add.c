#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#define NUM 20
#define NAME "/tmp/out"
#define SIZE 10

void sysio_func(){
	char tmp[SIZE];
	int fd;
	fd = open(NAME, O_RDWR);
	if (fd <0) {
		perror("open():");
		exit(-1);
	}
	lockf(fd, F_LOCK, 0);
	read(fd, tmp, SIZE);


	lockf(fd, F_ULOCK, 0);
	close(fd);
	exit(0);
}

int main(void){
	int i;
	pid_t pid;
	for (i = 0; i < NUM; i++) {
		pid = fork();
		if (pid==0) {
			sysio_func();
		}
	}

	for (i = 0; i < NUM; i++) {
		wait(NULL);
	}
	exit(0);
}
