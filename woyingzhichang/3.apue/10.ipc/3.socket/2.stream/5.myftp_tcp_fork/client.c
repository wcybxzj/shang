#include "myftp.h"
static int sd;

static void fsm_driver(FSM_ST *fsm){
	int len;
	char *err_str;
	switch(fsm->state){
		case STATE_SEND:
			len  = write(sd, &fsm->path_buf, sizeof(fsm->path_buf));
			if (len < 0) {
				if(errno == EINTR){
					fsm->state = STATE_SEND;
				}
				else{
					perror("write():");
					fsm->state = STATE_EX;
				}
			}else{
				fsm->state = STATE_RCV;
			}
			break;
		case STATE_RCV:
			len = read(sd, &fsm->data_buf, sizeof(fsm->data_buf));
			if (len < 0) {
				if(errno == EINTR){
					fsm->state = STATE_RCV;
				}else{
					fsm->errstr = "read():";
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

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if(sd < 0){
		perror("socket()");
		exit(-1);
	}

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(atoi(RCVPORT));
	if(inet_pton(AF_INET, argv[1], &saddr.sin_addr)!=1){
		perror("inet_pton()");
		exit(-1);
	}
	if(connect(sd, (void *)&saddr, sizeof(saddr)) < 0){
        perror("connect()");
        exit(-2);
    }   


	fsm.state = STATE_SEND;
	memset(&fsm.path_buf, 0x00, sizeof(fsm.path_buf));
	fsm.path_buf.mtype = MSG_PATH;
	strcpy(fsm.path_buf.path, argv[1]);
	strcpy(fsm.path_buf.path, argv[2]);
	while (1) {
		fsm_driver(&fsm);
	}

	exit(0);
}
