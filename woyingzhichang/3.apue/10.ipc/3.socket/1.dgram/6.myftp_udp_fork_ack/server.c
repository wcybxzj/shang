#include "myftp.h"
#define IPSIZE 16

static void fsm_driver(FSM_ST *fsm){
	int len, ret;
	static int num = 1;
	static int init = 0;
	if(init == 1 && fsm->state == STATE_RCV){
		while(1){
			ret = recvfrom(fsm->server_child_sd, \
					&fsm->data_buf, sizeof(fsm->data_buf), 0,\
					NULL, NULL);
			if (ret == -1) {
				if (errno == EINTR){
					continue;
				}
				perror("recvfrom !!!");
				exit(1);
			}else{
				break;
			}
		}
	}

	switch(fsm->state){
		case STATE_RCV:
			switch(fsm->data_buf.mtype){
				case MSG_PATH:
					fsm->fd = open(fsm->data_buf.data.pathmsg.path, O_RDWR);
					if(fsm->fd < 0){
						fsm->state = STATE_SEND;
						fsm->data_buf.mtype = MSG_ERR;
						fsm->data_buf.data.errmsg._errno_= errno;
					}else{
						init = 1;
						fsm->state = STATE_READ;
					}
					break;

				case MSG_ACK:
					//printf("pid:%d, server need ack num is %d\n", \
					//		getpid(), num);
					//printf("pid:%d, client give ack num is %d\n", \
					//		getpid(), fsm->data_buf.data.ackmsg.ack_data_num);
					if(fsm->data_buf.data.ackmsg.ack_data_num != num){
						//printf("pid:%d, server clinent exit\n", getpid());
						exit(-1);
					}else{
						fsm->state = STATE_READ;
						num++;
					}
					break;

				default:
					break;
			}
			break;

		case STATE_READ:
			printf("read\n");
			len = read(fsm->fd, \
					&fsm->data_buf.data.datamsg.data, DATASIZE);
			printf("pid:%d, read len is %d\n", getpid(), len);
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
				fsm->data_buf.data.datamsg.data_num = num;
			}
			break;

		case STATE_SEND:
			len = sendto(fsm->server_child_sd, &fsm->data_buf, sizeof(fsm->data_buf), 0 \
				, (struct sockaddr *) &fsm->client_addr, fsm->client_addr_len);

			printf("pid:%d, send num:%d\n", getpid(), num);
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
						fsm->state = STATE_RCV;
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

//测试:成功
//终端1:./server
//终端2:./client 127.0.0.1 /etc/services
int main(void){
	int ret;
	pid_t pid;
	char ip[IPSIZE];
	struct fsm_st fsm;
	struct sigaction sa;
	struct sockaddr_in laddr, raddr;

	fsm.server_sd = socket(AF_INET, SOCK_DGRAM, 0/*IPPROTO_UDP*/);
	if(fsm.server_sd < 0){
		perror("socket()");
		exit(-1);
	}

	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(atoi(RCVPORT));
	if(inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr.s_addr) != 1){
		perror("inent_pton()");
		exit(-1);
	}
	if(bind(fsm.server_sd, (void *)&laddr, sizeof(laddr))){
		perror("bind()");
		exit(-1);
	}

	for (;;) {
		fsm.state = STATE_RCV;
		fsm.client_addr_len = sizeof(fsm.client_addr);
		ret = recvfrom(fsm.server_sd, &fsm.data_buf, sizeof(fsm.data_buf), 0,\
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
	close(fsm.server_sd);
	exit(EXIT_SUCCESS); 
}
