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
		case STATE_SEND:
			len = msgsnd(fsm->msgid, &fsm->path_buf, \
					sizeof(fsm->path_buf) - sizeof(long), 0);
			if (len < 0) {
				if(errno == EINTR){
					fsm->state = STATE_SEND;
				}
				else{
					perror("msgsnd():");
					fsm->state = STATE_EX;
				}
			}else{
				fsm->state = STATE_RCV;
			}
			break;
		case STATE_RCV:
			len = msgrcv(fsm->msgid, &fsm->data_buf, \
					sizeof(fsm->data_buf)- sizeof(long) , \
					-MSG_ERR ,0);
			if (len < 0) {
				if(errno == EINTR){
					fsm->state = STATE_RCV;
				}else{
					fsm->errstr = "msgrcv():";
					fsm->state = STATE_EX;
				}
			}else{
				switch (fsm->data_buf.mtype){
					case MSG_DATA:
						fsm->state = STATE_OUTPUT;
						break;
					case MSG_ERR:
						printf("server said:%s\n", \
								strerror(fsm->data_buf.data.errmsg._errno_));
						fsm->state = STATE_T;
						break;
					case MSG_EOT:
						fsm->state = STATE_T;
						break;
					default:
						break;
				}
			}
			break;

		case STATE_OUTPUT:
			len = write(1, &fsm->data_buf.data.datamsg.data, \
					fsm->data_buf.data.datamsg.datalen);
			if (len<0) {
				if (errno == EINTR) {
					fsm->state = STATE_OUTPUT;
				}else{
					fsm->errstr = "write():";
					fsm->state = STATE_EX;
				}
			}else{
				fsm->state = STATE_RCV;
			}
			break;

		case STATE_T:
			exit(0);
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

int main(int argc, const char *argv[]){
	struct fsm_st fsm;
	key_t key;
	int msgid;
	if (argc<2) {
		printf("usgae:./client /etc/httpd/conf/httpd.conf\n");
		exit(-1);
	}

	key = ftok(KEYPATH, KEYPROJ);
	if(key < 0){
		perror("ftok():");
		exit(-1);
	}
	msgid = msgget(key,0);
	if(msgid < 0){
		perror("msgget():");
		exit(-1);
	}
	fsm.state = STATE_SEND;
	fsm.msgid = msgid;
	fsm.path_buf.mtype = MSG_PATH;
	strcpy(fsm.path_buf.path, argv[1]);

	while (1) {
		fsm_driver(&fsm);
	}
	exit(0);
}
