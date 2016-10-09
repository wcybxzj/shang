#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "myftp.h"

static void fsm_driver(FSM_ST *fsm){
	int len;
	switch(fsm->state){
		case STATE_RCV:
			len = msgrcv(fsm->msgid, &fsm->path_buf, \
					sizeof(fsm->path_buf)- sizeof(long) , \
					MSG_PATH ,0);
			if (len < 0) {
				if(errno == EINTR){
					fsm->state = STATE_RCV;
				}else{
					fsm->errstr = "msgrcv():";
					fsm->state = STATE_EX;
				}
			}else{
				fsm->fd = open(fsm->path_buf.path, O_RDWR);
				if(fsm->fd < 0){
					fsm->state = STATE_SEND;
					fsm->data_buf.mtype = MSG_ERR;
					fsm->data_buf.data.errmsg._errno_= errno;
				}else{
					fsm->state = STATE_READ;
				}
			}
			break;

		case STATE_READ:
			len = read(fsm->fd, \
					&fsm->data_buf.data.datamsg.data, DATASIZE);
			if(len < 0){
				if (errno == EINTR) {
					fsm->state = STATE_READ;
				}else{
					fsm->state = STATE_SEND;
					fsm->data_buf.mtype = MSG_ERR;
					fsm->data_buf.data.errmsg._errno_= errno;
				}
			}else if(len==0){
				fsm->state = STATE_SEND;
				fsm->data_buf.mtype = MSG_EOT;
			}else{
				fsm->state = STATE_SEND;
				fsm->data_buf.mtype = MSG_DATA;
				fsm->data_buf.data.datamsg.datalen = len;
			}
			break;

		case STATE_SEND:
			len = msgsnd(fsm->msgid, &fsm->data_buf, \
					sizeof(fsm->data_buf) - sizeof(long), 0);
			if (len < 0) {
				if(errno == EINTR){
					fsm->state = STATE_SEND;
				}
				else{
					fsm->state = STATE_EX;
					fsm->errstr = "msgsnd():";
				}
			}else{
				switch (fsm->data_buf.mtype){
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
			perror(fsm->errstr);
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
