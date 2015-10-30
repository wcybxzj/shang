#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>

#include "proto.h"


struct fsm_st {
	int state;
	msg_path_t sbuf;
	msg_s2c_u rbuf;
	char *errstr;
};

static int msgid;

static void fsm_driver(struct fsm_st *fsm)
{
	switch(fsm->state) {
		case STATE_RCV:
			if(msgrcv(msgid, &fsm->rbuf, sizeof(fsm->rbuf)-sizeof(long), 0, 0) < 0) {
				if(errno == EINTR) {
					fsm->state = STATE_RCV;
				}else {
					fsm->errstr = "msgrcv()";
					fsm->state = STATE_Ex;
				}
			}else {
				switch(fsm->rbuf.mtype) {
					case MSG_DATA:
						fprintf(stdout,"%s",fsm->rbuf.datamsg.data);
						fsm->state = STATE_RCV;
						break;
					case MSG_ERR:
						printf("%d\n", fsm->rbuf.errmsg.errno_);
						fsm->errstr = "error";
						fsm->state = STATE_Ex;
						break;
					case MSG_EOT:
						fsm->state = STATE_T;
						break;
					default:
						fsm->state = STATE_SND;
						break;
					}
			}
			break;
		case STATE_SND:
			if(msgsnd(msgid, &fsm->sbuf, sizeof(fsm->sbuf)-sizeof(long), 0) < 0) {
				if(errno == EINTR) {
					fsm->state = STATE_SND;
				}else {
					fsm->errstr = "msgsnd()";
					fsm->state = STATE_Ex;
				}
			}else {
				fsm->state = STATE_RCV;
			}
			break;
		case STATE_Ex:
			perror(fsm->errstr);
			fsm->state = STATE_T;
			break;
		case STATE_T:
			break;
		default:
			abort();
	}

	return;
}

int main(int argc, char **argv)
{
	if(argc < 2) {
		fprintf(stderr, "Usage...\n");
		exit(1);
	}

	key_t key;
	struct fsm_st fsm;

	key = ftok(KEYPATH, KEYPROJ);
	if(key < 0) {
		perror("ftok()");
		exit(1);
	}

	msgid = msgget(key, 0);
	if(msgid < 0) {
		perror("msgget()");
		exit(1);
	}
	
	fsm.state = STATE_SND;
	fsm.sbuf.mtype = MSG_PATH;
	strcpy(fsm.sbuf.path, argv[1]);

	while(fsm.state != STATE_T) {
		fsm_driver(&fsm);
	}

	exit(0);
}












