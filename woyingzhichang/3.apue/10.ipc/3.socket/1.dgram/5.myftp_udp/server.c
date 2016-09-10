#include "myftp.h"
#define IPSIZE 16

static void fsm_driver(FSM_ST *fsm){
	int len;
	switch(fsm->state){
		case STATE_RCV:
			printf("pathname:%s",fsm->path_buf.path);
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
			len = sendto(fsm->server_child_sd, &fsm->data_buf, sizeof(fsm->data_buf), 0 \
				, (struct sockaddr *) &fsm->client_addr, fsm->client_addr_len);

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
	int ret;
	int sd;
	pid_t pid;
	char ip[IPSIZE];
	struct fsm_st fsm;
	struct sigaction sa;
	struct sockaddr_in laddr, raddr;

	sd = socket(AF_INET, SOCK_DGRAM, 0/*IPPROTO_UDP*/);
	if(sd < 0){
		perror("socket()");
		exit(-1);
	}

	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(atoi(RCVPORT));
	if(inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr.s_addr) != 1){
		perror("inent_pton()");
		exit(-1);
	}
	if(bind(sd, (void *)&laddr, sizeof(laddr))){
		perror("bind()");
		exit(-1);
	}

	for (;;) {
		fsm.state = STATE_RCV;
		fsm.client_addr_len = sizeof(fsm.client_addr);
		ret = recvfrom(sd, &fsm.path_buf, sizeof(fsm.path_buf), 0,\
				 (void *)&fsm.client_addr, &fsm.client_addr_len);

		if (ret == -1) {
			if (errno == EINTR)         /* Interrupted by SIGCHLD handler? */
				continue;               /* ... then restart msgrcv() */
			perror("recvfrom !!!");           /* Some other error */
			exit(1);
			break;                      /* ... so terminate loop */
		}

		pid = fork();                   /* Create child process */
		if (pid == -1) {
			perror("fork");
			break;
		}

		if (pid == 0) {                 /* Child handles request */
			fsm.server_child_sd = socket(AF_INET, SOCK_DGRAM, 0/*IPPROTO_UDP*/);
			if(fsm.server_child_sd < 0){
				perror("socket()");
				exit(-1);
			}
			while (1) {
				fsm_driver(&fsm);
			}
			exit(EXIT_SUCCESS);
		}
		/* Parent loops to receive next client request */
	}
	close(sd);
	exit(EXIT_SUCCESS); 
}
