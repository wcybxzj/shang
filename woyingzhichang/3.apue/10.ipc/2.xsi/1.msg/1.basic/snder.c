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

	msgid = msgget(key,0);
	if(msgid < 0){
		perror("msgget");
		exit(-1);
	}

	buf.mtype = ONE;
	strcpy(buf.name, "ybx");
	buf.math=100;
	buf.chinese=200;

	len  = msgsnd(msgid, &buf, sizeof(buf)- sizeof(long), 0);
	if (len < 0) {
		perror("msgsnd():");
		exit(-1);
	}

	exit(0);
}
