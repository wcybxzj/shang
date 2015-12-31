#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "proto1.h"


struct fsm_st{
	int state; 
	int msgid;    
	msg_path_t sbuf;
	union msg_s2c_un rbuf;
	char *errstr;
};

static void fsm_driver(struct fsm_st *fsm){
	int ret;
	switch(fsm->state){
		case STATE_RECV:
			ret = msgrcv(fsm->msgid, &fsm->rbuf,
					sizeof(fsm->rbuf)-sizeof(long),0 ,0);
			if (ret < 0) {
				if (errno == EINTR) {
					fsm->state = STATE_RECV;
				}else{
					fsm->state = STATE_EX;
					fsm->errstr = "msgrcv";
				}
			}else{
				switch(fsm->rbuf.mtype){
					case MSG_DATA:
						printf("%s\n", fsm->rbuf.datamsg.data);
						fsm->state = STATE_RECV;
						break;
					case MSG_EOT:
						fsm->state = STATE_T;
						break;
					case MSG_ERR:
						fsm->state = STATE_EX;
						fsm->errstr = "error:";
						errno = fsm->rbuf.errmsg.errno_;
						break;
					default:
						fsm->state = STATE_SEND;
						break;
				}
			}
			break;
		case STATE_SEND:
			ret = msgsnd(fsm->msgid, &fsm->sbuf,
					sizeof(fsm->sbuf)-sizeof(fsm->sbuf.mtype), 0);
			if (ret < 0) {
				if (errno == EINTR) {
					fsm->state = STATE_SEND;
				}else{
					fsm->state = STATE_EX;
					fsm->errstr = "msgsnd";
				}
			}else{
				fsm->state = STATE_RECV;
			}
			break;
		case STATE_EX:
			perror(fsm->errstr);
			fsm->state = STATE_T;
			break;
		case STATE_T:
			break;
		default:
			break;
	}
}

int main(int argc, const char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "uasge ...\n");
		exit(1);
	}

	struct fsm_st fsm;
	key_t key;
	int  msgid;

	key = ftok(KEYPATH, KEYPROJ);
	if (key < 0) {
		perror("ftok()");
		exit(1);
	}

	msgid = msgget(key, 0);

	if (msgid < 0) {
		perror("msgget()");
		exit(1);
	}

	fsm.state = STATE_SEND;
	fsm.msgid = msgid;
	fsm.sbuf.mtype = MSG_PATH;
	strcpy(fsm.sbuf.path, argv[1]);
	
	while (fsm.state != STATE_T) {
		fsm_driver(&fsm);
	}

	return 0;
}
