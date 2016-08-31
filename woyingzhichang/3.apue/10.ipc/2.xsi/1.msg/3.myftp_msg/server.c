#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "proto3.h"
typedef struct fsm_st{
	int state;
	int msgid;
	int fd;
	char *errstr;
	msg_path_t rbuf;
	msg_s2c_t sbuf;
} FSM_ST;

static void fsm_driver(FSM_ST *fsm){
	int len;
	switch(fsm->state){
		case STATE_RCV:
			len = msgrcv(fsm->msgid, &fsm->rbuf.path, \
					sizeof(path_t)- sizeof(long) , \
					0 ,0);
			if (len < 0) {
				if(errno == EINTR){
					fsm->state = STATE_RCV;
				}else{
					fsm->errstr = "msgrcv():";
					fsm->state = STATE_EX;
				}
			}else{
				fsm->fd = open(fsm->fd, O_RDWR);
				if(fsm->fd < 0){
					fsm->state = STATE_SEND;
					fsm->sbuf.s2c.mtype = MSG_ERR;
					fsm->sbuf.s2c.errmsg._errno_= errno;
				}else{
					fsm->state = STATE_READ;
				}
			}
			break;

		case STATE_READ:
			len = read(fsm->fd, \
					&fsm->sbuf.s2c.datamsg.data, DATASIZE);
			if(len < 0){
				if (errno == EINTR) {
					fsm->state = STATE_READ;
				}else{
					fsm->errstr = "open():";
					fsm->state = STATE_EX;
				}
			}else if(len==0){
				fsm->state = STATE_SEND;
				fsm->sbuf.s2c.mtype = MSG_EOT;
			}else{
				fsm->state = STATE_SEND;
				fsm->sbuf.s2c.datamsg.datalen = len;
			}
			break;

		case STATE_SEND:
			len = msgsnd(msgid, &fsm->sbuf, \
					sizeof(fsm->sbuf) - sizeof(long), 0);
			if (len < 0) {
				if(errno == EINTR){
					fsm->state = STATE_SEND;
				}
				else{
					fsm->state = STATE_Ex;
					fsm->errstr = "msgsnd():";
				}
			}else{
				switch (fsm->state){
					case MSG_DATA:
						fsm->state = STATE_READ;
					break;
					case MSG_ERR:
						fsm->state = STATE_RCV;
					break;
					case MSG_EOT:
						fsm->state = STATE_RCV;
					break;
					default:
					break;
				}
			}
			break;

		case STATE_EX:
			peror(fsm->errstr);
			fsm->state = STATE_RCV;
			break;

		default:
			exit(0);
			break;

	}

}

int main(void){
	struct fsm_st fsm;
	key_t key;
	int msgid;
	key = ftok(KEYPATH, KEYPROJ);
	if(key < 0){
		perror("ftok():");
		exit(-1);
	}
	msgid = msgget(key, IPC_CREAT|0666);
	if(msgid < 0){
		perror("msgget():");
		exit(-1);
	}
	fsm.state = STATE_RCV;
	fsm.msgid = msgid;
	while (1) {
		fsm_driver(&fsm);
	}
	exit(0);
}
