#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>

#include "proto.h"

int main(void){
	int i;
	pid_t pid;
	key_t key;
	int msgid;
	struct msg_st buf;
	ssize_t len;
	long arr[2] = {ONE, TWO};

	key = ftok(KEYPATH, KEYPROJ);
	if(key < 0){
		perror("ftok():");
		exit(-1);
	}

	msgid = msgget(key,0);
	if(msgid < 0){
		perror("msgget");
		exit(-1);
	}

	for(i=0; i<2; i++){
		pid = fork();
		if(pid == 0){
			while (1) {
				buf.mtype = arr[i];
				sprintf(buf.name, "ybx%d", arr[i]);
				buf.math=100+arr[i];
				buf.chinese=200+arr[i];
				len  = msgsnd(msgid, &buf, sizeof(buf)- sizeof(long), 0);
				if (len < 0) {
					perror("msgsnd():");
					exit(-1);
				}
				printf("%d, send data\n", arr[i]);
				sleep(1);
			}
			exit(0);
		}
	}

	for(i=0; i<2; i++){
		wait(NULL);
	}
	exit(0);
}
