#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


#include "proto.h"

struct fsm_st {
	int state;
	msg_path_t rbuf;
	msg_s2c_u sbuf;	
	char *errstr;
};

static int msgid;

static void fsm_driver(struct fsm_st *fsm)
{
	int fd;

	switch(fsm->state) {
		case STATE_RCV:
			if(msgrcv(msgid, &fsm->rbuf, sizeof(fsm->rbuf)-sizeof(long), \
						MSG_PATH, 0) < 0) {
				if(errno == EINTR) {
					fsm->state = STATE_RCV;
				}else {
					fsm->errstr = "msgrcv()";
					fsm->state = STATE_Ex;
				}
			}else {
				if(fsm->rbuf.mtype == MSG_PATH) {
					puts(fsm->rbuf.path);
					fd = open(fsm->rbuf.path, O_RDONLY);
					if(fd < 0) {
						fsm->sbuf.mtype = MSG_ERR;
						fsm->sbuf.errmsg.errno_ = errno;
					}
					fsm->state = STATE_SND;
				}else {
					fsm->state = STATE_RCV;
				}
			}
			break;
		case STATE_SND:
			if(fsm->sbuf.mtype != MSG_ERR) {
				memset(fsm->sbuf.datamsg.data, 0x00, DATASIZE);
				fsm->sbuf.datamsg.datasize = \
							read(fd, fsm->sbuf.datamsg.data, DATASIZE);
				if(fsm->sbuf.datamsg.datasize == 0) {
					fsm->sbuf.mtype = MSG_EOT;
					while(msgsnd(msgid, &fsm->sbuf, \
						  sizeof(fsm->sbuf)-sizeof(long), 0) < 0) {
						if(errno == EINTR) {
							continue;
						}
						fsm->errstr = "msgsnd()";
						fsm->state = STATE_Ex;
						return;
					}
					fsm->state = STATE_RCV;
				}else if(fsm->sbuf.datamsg.datasize < 0) {
					if(errno == EINTR) {
						fsm->state = STATE_SND;
					}else {
						fsm->errstr = "read()";
						fsm->state = STATE_Ex;
					}
				}else {
					fsm->sbuf.mtype = MSG_DATA;
					while(msgsnd(msgid, &fsm->sbuf, \
							sizeof(fsm->sbuf)-sizeof(long), 0) < 0) {
						if(errno == EINTR) {
							continue;
						}
						fsm->errstr = "msgsnd()";
						fsm->state = STATE_Ex;
						return;
					}
					fsm->state = STATE_SND;
				}
			}else {
				if(msgsnd(msgid, &fsm->sbuf, \
							sizeof(fsm->sbuf)-sizeof(long), 0) < 0) {
					if(errno == EINTR) {
						fsm->state = STATE_SND;
					}else {
						fsm->errstr = "msgsnd()";
						fsm->state = STATE_Ex;
					}
				}else {
					fsm->sbuf.mtype = 0;
					fsm->state = STATE_RCV;
				}
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

int main()
{
	key_t key;
	struct fsm_st fsm; 

	key = ftok(KEYPATH, KEYPROJ);
	if(key < 0) {
		perror("ftok()");
		exit(1);
	}

	msgid = msgget(key, IPC_CREAT | 0600);
	if(msgid < 0) {
		perror("msgget()");
		exit(1);
	}

	fsm.state = STATE_RCV;

	while(fsm.state != STATE_T) {
		fsm_driver(&fsm);
	}

	msgctl(msgid, IPC_RMID, NULL);

	exit(0);
}














