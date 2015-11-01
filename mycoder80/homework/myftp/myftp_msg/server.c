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
	msg_path_t rbuf;
	union msg_s2c_un sbuf;
	char *errstr;
};

/*
 * STATE_RECV:msgrcv必须指定信息类型MSG_PATH，虽然第一次收PATH不制定没关系，
 * 但是发数据，EOT切换到STATE_RECV,因为刚发的结构体包大于path包，所以就会报错msgrcv error: Argument list too long 
 * 也可以EOT发完sleep 1秒，让client端把数据收走,但是这样不好
*/
static void fsm_driver(struct fsm_st *fsm){
	int ret;
	int fd;
	switch(fsm->state){
		case STATE_RECV:
			//printf("block on recv\n");//debug
			//ret = msgrcv(fsm->msgid ,&fsm->rbuf, sizeof(fsm->rbuf)-sizeof(long),0,0);
			ret = msgrcv(fsm->msgid ,&fsm->rbuf, sizeof(fsm->rbuf)-sizeof(long),MSG_PATH,0);
			if (ret < 0) {
				if (errno == EINTR) {
					fsm->state = STATE_RECV;
				}else{
					fsm->errstr = "server state_recv msgrcv()";
					fsm->state = STATE_EX; 
				}
			}else{
				if (fsm->rbuf.mtype == MSG_PATH) {
					fd = open(fsm->rbuf.path, O_RDONLY);
					if (fd < 0) {
						fsm->sbuf.mtype = MSG_ERR;
						fsm->sbuf.errmsg.errno_ = errno;
					}
					fsm->state = STATE_SEND;
				}else{
					fsm->state = STATE_RECV;
				}
			}
			break;
		case STATE_SEND:
			if (fsm->sbuf.mtype != MSG_ERR) {
				memset(fsm->sbuf.datamsg.data, 0x00, DATASIZE);
				fsm->sbuf.datamsg.datasize = read(fd, fsm->sbuf.datamsg.data, DATASIZE);
				//printf("%s\n", fsm->sbuf.datamsg.data);//debug
				if (fsm->sbuf.datamsg.datasize==0) {
					fsm->sbuf.mtype = MSG_EOT;
					while (msgsnd(fsm->msgid, &fsm->sbuf,
						sizeof(fsm->sbuf) - sizeof(long), 0) < 0) {
						if (errno == EINTR) {
							continue;
						}
						fsm->state = STATE_EX;
						fsm->errstr= "server state_send msg_eot msgsnd()";
						return;
					}
					fsm->state = STATE_RECV;
					//sleep(1);
				}else if(fsm->sbuf.datamsg.datasize < 0){
					if (errno == EINTR) {
						fsm->state = STATE_SEND;
					}else{
						fsm->state = STATE_EX;
						fsm->errstr = "server state_send read()";
					}
				}else{
					fsm->sbuf.mtype = MSG_DATA;
					while (msgsnd(fsm->msgid, &fsm->sbuf,
						sizeof(fsm->sbuf) - sizeof(long), 0) < 0) {
						if (errno==EINTR) {
							continue;
						}
						fsm->state = STATE_EX;
						fsm->errstr = "server state_send msg_data msgsnd()";
						return;
					}
				}
			}else{
				if(msgsnd(fsm->msgid, &fsm->sbuf,
					sizeof(fsm->sbuf) - sizeof(long),0) < 0) {
					if (errno == EINTR) {
						fsm->state = STATE_SEND;
					}else{
						fsm->state = STATE_EX;
						fsm->errstr = "msgsnd";
					}
				}else{
					fsm->state = STATE_RECV;
				}
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

void relayer(int msgid)
{
	struct fsm_st fsm;
	fsm.state = STATE_RECV;
	fsm.msgid = msgid;

	while (fsm.state != STATE_T) {
		fsm_driver(&fsm);
	}
}

int main(int argc, const char *argv[])
{
	key_t key;
	int  msgid;

	key = ftok(KEYPATH, KEYPROJ);
	if (key < 0) {
		perror("ftok()");
		exit(1);
	}

	msgid = msgget(key, IPC_CREAT|0600);

	if (msgid < 0) {
		perror("msgget()");
		exit(1);
	}

	relayer(msgid);

	msgctl(msgid, IPC_RMID, NULL);

	return 0;
}
