#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>

#include "proto.h"

//删除消息队列
int main(void){
	key_t key;
	int msgid;
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
	msgctl(msgid, IPC_RMID, NULL);
	exit(0);
}
