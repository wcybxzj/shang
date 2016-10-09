#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SIZE 1024
int main(){
	int len;
	int pid;
	int pd[2];
	char str[SIZE];
	if(pipe(pd) < 0){
		perror("perror():");
		exit(0);
	}

	pid = fork();
	if(pid == 0){
		close(pd[1]);
		len = read(pd[0], str, SIZE);
		printf("child read from pipe:%s\n", str);
		exit(0);
	}else if(pid >0){
		close(pd[0]);
		write(pd[1],"abc!", 5);
		wait(NULL);
	}
	exit(0);
}
