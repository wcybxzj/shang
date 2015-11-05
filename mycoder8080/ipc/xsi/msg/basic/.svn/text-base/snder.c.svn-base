#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#include "proto.h"

int main()
{
	key_t key;
	struct msg_st sbuf;
	int msgid;

	key = ftok(KEYPATH,KEYPROJ);
	if(key < 0)
	{
		perror("ftok()");
		exit(1);
	}

	msgid = msgget(key,0);
	if(msgid < 0)
	{
		perror("msgget()");
		exit(1);
	}

	sbuf.mtype = MYMSG;
	strcpy(sbuf.name,"Alan");
	sbuf.math = rand()%100;
	sbuf.chinese = rand()%100;
	if(msgsnd(msgid,&sbuf,sizeof(sbuf)-sizeof(long),0) < 0)
	{
		perror("msgsnd()");
		exit(1);
	}

	puts("ok!");
	
	exit(0);
}




