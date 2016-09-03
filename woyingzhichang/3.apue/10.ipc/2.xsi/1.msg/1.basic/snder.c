#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "proto.h"

//每次发1个消息
void msgsnd_one()
{
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

	srand(time(NULL));
	buf.mtype = ONE;
	strcpy(buf.name, "ybx");
	buf.math=rand()%100;
	buf.chinese=rand()%100;

	len  = msgsnd(msgid, &buf, sizeof(buf)- sizeof(long), 0);
	if (len < 0) {
		perror("msgsnd():");
		exit(-1);
	}
}

int main(void){
	msgsnd_one();
	exit(0);
}
