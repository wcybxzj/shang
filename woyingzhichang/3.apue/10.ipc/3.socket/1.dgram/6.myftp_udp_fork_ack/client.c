#include "myftp.h"

static void fsm_driver(FSM_ST *fsm){
	int len;
	char *err_str;
	switch(fsm->state){
		case STATE_SEND:
			switch(fsm->data_buf.mtype){
				case MSG_PATH:
					len = sendto(fsm->client_sd, &fsm->data_buf, sizeof(fsm->data_buf), \
							0, (void *) &fsm->server_addr, sizeof(fsm->server_addr));
					break;

				case MSG_ACK:
					len = sendto(fsm->client_sd, &fsm->data_buf, sizeof(fsm->data_buf), \
							0, (void *) &fsm->server_child_addr, fsm->server_child_addr_len);
					break;

				default:
					fsm->state = STATE_T;
					break;
			}

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
			fsm->server_child_addr_len = sizeof(fsm->server_child_addr);
			len = recvfrom(fsm->client_sd, &fsm->data_buf, sizeof(fsm->data_buf), 0,\
					(void *)&fsm->server_child_addr, &fsm->server_child_addr_len);
			printf("recv data packsize len:%d\n", len);
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
			switch(fsm->data_buf.mtype){
				case MSG_DATA:
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
						fsm->data_buf.mtype = MSG_ACK;
						fsm->data_buf.data.ackmsg.ack_data_num = fsm->data_buf.data.datamsg.data_num;
						fsm->state = STATE_SEND;
					}
					break;
				case MSG_ERR:
					err_str = strerror(fsm->data_buf.data.errmsg._errno_);
					len = write(1, err_str,strlen(err_str));
					if (len<0) {
						if (errno == EINTR) {
							fsm->state = STATE_OUTPUT;
						}else{
							fsm->errstr = "write():";
							fsm->state = STATE_EX;
						}
					}else{
						fsm->state = STATE_T;
					}
					break;
				default:
					break;
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

	static void             /* SIGCHLD handler */ 
func(int sig) 
{ 
	int savedErrno; 
	savedErrno = errno;                 /* waitpid() might change 'errno' */ 
	exit(0);
	errno = savedErrno; 
} 

//./client 192.168.91.11 /etc/hosts
int main(int argc, const char *argv[]){
	int len;
	struct fsm_st fsm;
	struct sigaction sa;
	struct sockaddr_in saddr;
	if (argc!=3) {
		printf("usgae:./client 192.168.91.11 /etc/httpd/conf/httpd.conf\n");
		exit(-1);
	}

	fsm.client_sd = socket(AF_INET, SOCK_DGRAM, 0/*IPPROTO_UDP*/);
	if(fsm.client_sd < 0){
		perror("socket()");
		exit(-1);
	}

	fsm.state = STATE_SEND;
	memset(&fsm.data_buf, 0x00, sizeof(fsm.data_buf));
	fsm.data_buf.mtype = MSG_PATH;
	strcpy(fsm.data_buf.data.pathmsg.path, argv[2]);
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(atoi(RCVPORT));
	if(inet_pton(AF_INET, argv[1], &saddr.sin_addr)!=1){
		perror("inet_pton()");
		exit(-1);
	}
	fsm.server_addr = saddr;

	while (1) {
		fsm_driver(&fsm);
	}

	exit(0);
}
