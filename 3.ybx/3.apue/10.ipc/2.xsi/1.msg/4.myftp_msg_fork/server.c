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

static void fsm_driver(FSM_ST *fsm){
	int len;
	switch(fsm->state){
		case STATE_RCV:
			fsm->fd = open(fsm->path_buf.path, O_RDWR);
			if(fsm->fd < 0){
				fsm->state = STATE_SEND;
				fsm->data_buf.mtype = MSG_ERR;
				fsm->data_buf.data.errmsg._errno_= errno;
			}else{
				fsm->state = STATE_READ;
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
			//sleep(1);//方便观察多个客户端
			len = msgsnd(fsm->path_buf.client_id, &fsm->data_buf, \
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
						exit(0);
					break;
					case MSG_EOT:
						exit(0);
					break;
					default:
					break;
				}
			}
			break;

		case STATE_EX:
			printf("pid:%d, errstr:%s", getpid(), fsm->errstr);
			exit(0);
			break;

		default:
			exit(0);
			break;

	}

}

static void             /* SIGCHLD handler */ 
grimReaper(int sig) 
{ 
    int savedErrno; 
    savedErrno = errno;                 /* waitpid() might change 'errno' */ 
    while (waitpid(-1, NULL, WNOHANG) > 0) 
        continue; 
    errno = savedErrno; 
} 

//server多进程可以同时支持多个clinet
//终端1:./server
//终端2:./client /etc/services
//终端3:./client /etc/httpd/conf/httpd.conf

//观察方法:
//watch -n1 'ipcs -q'
//watch -n1 'ps -ef|grep client'
//watch -n1 'ps -ef|grep server'

//观察管道最大字节限制 65535字节
//server:msgsnd注释掉sleep
//client:msgrcv使用sleep(10000)

//观察client将一个写满的消息队列数据一点点读出来
//server:msgsnd注释掉sleep
//client:msgrcv使用sleep(1)
//然后Ctrl+C server进程,因为子进程没脱离控制终端也会被中止
int main(void){
	struct fsm_st fsm;
	int server_id;
	int msgLen;
	struct sigaction sa;
	pid_t pid;

	server_id = msgget(SERVER_KEY, IPC_CREAT|0666);
	if(server_id < 0){
		perror("msgget():");
		exit(-1);
	}
	printf("server_id:%d\n", server_id);

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grimReaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
		exit(0);
	}

    for (;;) {
		//memset(&fsm, 0x00, sizeof(fsm));
		fsm.state = STATE_RCV;
		fsm.server_id = server_id;
        msgLen = msgrcv(server_id, &fsm.path_buf, \
					sizeof(fsm.path_buf)-sizeof(long) , \
					0,0);
        if (msgLen == -1) {
            if (errno == EINTR)         /* Interrupted by SIGCHLD handler? */
                continue;               /* ... then restart msgrcv() */
            perror("msgrcv!!!");           /* Some other error */
			exit(1);
            break;                      /* ... so terminate loop */
        }

        pid = fork();                   /* Create child process */
        if (pid == -1) {
            perror("fork");
            break;
        }

        if (pid == 0) {                 /* Child handles request */
			while (1) {
				fsm_driver(&fsm);
			}
            exit(EXIT_SUCCESS);
        }
        /* Parent loops to receive next client request */
    }

    if (msgctl(server_id, IPC_RMID, NULL) == -1) 
        perror("msgctl"); 
    exit(EXIT_SUCCESS); 
}
