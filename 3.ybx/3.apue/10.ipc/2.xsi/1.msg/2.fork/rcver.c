#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>

#include "proto.h"

int main(void){
	key_t key;
	int i, msgid;
	struct msg_st buf;
	ssize_t len;
	pid_t pid;
	long arr[2] ={ONE, TWO};

	key = ftok(KEYPATH, KEYPROJ);
	if(key < 0){
		perror("ftok():");
		exit(-1);
	}

	msgid = msgget(key, IPC_CREAT|0644);
	if(msgid < 0){
		perror("msgget");
		exit(-1);
	}


	for(i=0; i<2; i++){
		pid = fork();
		if(pid == 0){
			while (1) {
				len  = msgrcv(msgid, &buf, sizeof(buf)- sizeof(long), arr[i], 0);
				if (len < 0) {
					perror("msgrcv():");
					exit(-1);
				}
				printf("mtype:%d,name:%s,math:%d,chinese:%d\n",\
						arr[i], buf.name , buf.math, buf.chinese);
			}
			eixt(0);
		}
	}

	for(i=0; i<2; i++){
		wait(NULL);
	}
	msgctl(msgid, IPC_RMID, NULL);
	exit(0);
}
