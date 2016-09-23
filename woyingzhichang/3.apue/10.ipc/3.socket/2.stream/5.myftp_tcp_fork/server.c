#include "myftp.h"
#define IPSIZE 16

static void fsm_driver(FSM_ST *fsm){
	int len;
	switch(fsm->state){
		case STATE_RCV:
			len = read(fsm->newsd, &fsm->path_buf, PATHSIZE);
			if(len < 0){
				if (errno == EINTR) {
					fsm->state = STATE_RCV;
				}else{
					fsm->state = STATE_EX;
					fsm->errstr = "read():";
				}
				break;
			}else if(len==0){
				fsm->state = STATE_RCV;
				break;
			}
			//printf("pathname:%s",fsm->path_buf.path);
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
			printf("read\n");
			len = read(fsm->fd, \
					&fsm->data_buf.data.datamsg.data, DATASIZE);
			printf("pid:%d, len is %d\n", getpid(), len);
			//printf("%s\n", fsm->data_buf.data.datamsg.data);
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
			len = write(fsm->newsd, &fsm->data_buf, sizeof(fsm->data_buf));
			if (len < 0) {
				if(errno == EINTR){
					fsm->state = STATE_SEND;
				}
				else{
					fsm->state = STATE_EX;
					fsm->errstr = "write():";
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

//tcp自己带ack和滑动窗口不要在写myftp_tcp_fork_ack
int main(void){
	int sd;
	pid_t pid;
	char ip[IPSIZE];
	struct fsm_st fsm;
	struct sockaddr_in laddr, raddr;
	socklen_t rlen;

	sd = socket(AF_INET, SOCK_STREAM, 0);
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


	listen(sd, 200);
	rlen = sizeof(raddr);
	for (;;) {
		fsm.state = STATE_RCV;
		fsm.newsd = accept(sd, (void *)&raddr, &rlen);
		if(fsm.newsd<0){
			if(errno == EAGAIN ||errno == EINTR){
				continue;
			}   
			perror("accept()");
			exit(-2);
		}   


		pid = fork();
		if (pid == -1) {
			perror("fork");
			break;
		}else if (pid == 0) {
			while (1) {
				fsm_driver(&fsm);
			}
			exit(EXIT_SUCCESS);
		}
	}
	exit(EXIT_SUCCESS); 
}
