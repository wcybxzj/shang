#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#include "myftp.h"
static int client_id;

static void fsm_driver(FSM_ST *fsm){
	int len;
	char *err_str;
	switch(fsm->state){
		case STATE_SEND:
			len = msgsnd(fsm->server_id, &fsm->path_buf, \
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
			sleep(1);//方便观察多个客户端
			len = msgrcv(fsm->path_buf.client_id, &fsm->data_buf, \
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
						fsm->state = STATE_OUTPUT;
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
			if(fsm->data_buf.mtype == MSG_DATA){
				len = write(1, &fsm->data_buf.data.datamsg.data, \
						fsm->data_buf.data.datamsg.datalen);
			}else if(fsm->data_buf.mtype == MSG_ERR){
				err_str = strerror(fsm->data_buf.data.errmsg._errno_);
				len = write(1, err_str,strlen(err_str));
			}
			if (len<0) {
				if (errno == EINTR) {
					fsm->state = STATE_OUTPUT;
				}else{
					fsm->errstr = "write():";
					fsm->state = STATE_EX;
				}
			}else{
				if(fsm->data_buf.mtype == MSG_DATA){
					fsm->state = STATE_RCV;
				}else if(fsm->data_buf.mtype == MSG_ERR){
					fsm->state = STATE_T;
				}
			}
			break;

		case STATE_T:
			exit(0);
			break;

		case STATE_EX:
			perror(fsm->errstr);
			fsm->state = STATE_T;
			break;

		default:
			exit(0);
			break;
	}
}

static void removeQueue(void)
{
    if (msgctl(client_id, IPC_RMID, NULL) == -1) {
        perror("msgctl");
	}
}

static void             /* SIGCHLD handler */ 
func(int sig) 
{ 
    int savedErrno; 
    savedErrno = errno;                 /* waitpid() might change 'errno' */ 
	exit(0);
    errno = savedErrno; 
} 

int main(int argc, const char *argv[]){
	struct fsm_st fsm;
	key_t key;
	int server_id;
	struct sigaction sa;
	if (argc<2) {
		printf("usgae:./client /etc/httpd/conf/httpd.conf\n");
		exit(-1);
	}

	//意外中断变成合理化退出进程 执行钩子
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = func;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
		exit(0);
	}

	server_id = msgget(SERVER_KEY, 0);
	if(server_id < 0){
		perror("msgget():");
		exit(-1);
	}

	client_id = msgget(IPC_PRIVATE, 0666);
	if(client_id < 0){
		perror("msgget():");
		exit(-1);
	}

	fsm.state = STATE_SEND;
	fsm.server_id = server_id;
	memset(&fsm.path_buf, 0x00, sizeof(fsm.path_buf));
	fsm.path_buf.mtype = MSG_PATH;
	fsm.path_buf.client_id = client_id;
	strcpy(fsm.path_buf.path, argv[1]);

	if(atexit(removeQueue) != 0){
		perror("atexit()");
	}

	while (1) {
		fsm_driver(&fsm);
	}

	exit(0);
}
