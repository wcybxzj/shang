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
	int msgid;
	struct msg_st buf;
	ssize_t len;
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

	while (1) {
		len  = msgrcv(msgid, &buf, sizeof(buf)- sizeof(long), 0, 0);
		if (len < 0) {
			perror("msgrcv():");
			exit(-1);
		}
		if(buf.mtype == ONE){
			printf("name:%s\n", buf.name);
			printf("math:%d\n", buf.math);
			printf("chinese:%d\n", buf.chinese);
		}
	}

	msgctl(msgid, IPC_RMID, NULL);
	exit(0);
}
