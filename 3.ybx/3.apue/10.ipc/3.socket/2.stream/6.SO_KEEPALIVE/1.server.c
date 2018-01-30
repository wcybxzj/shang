#include "header.h"

void worker(int newsd)
{
	int ret;
	char buf[1000]={0};
	while (1) {
		ret = read(newsd, buf, sizeof(buf));
		if (ret==0) {
			printf("client close\n");
			close(newsd);
			break;
		}else if(ret<0){
			printf("error:%s\n",strerror(errno));
			break;
		}else{
			printf("wrire!!!!!!!!!!!!\n");
			write(STDOUT_FILENO, buf,ret);
		}
	}
}

int main(int argc, const char *argv[])
{
	if (argc!=4) {
		printf("./a.out 127.0.0.1 1234 yes\n");
		exit(1);
	}

	int ret;
	int use_keepalive=0;

	if (strcmp(argv[3],"yes")==0) {
		printf("use keep-alive\n");
		use_keepalive=1;
	}else{
		printf("not use keep_alive\n");
	}

	int sd, newsd;
	struct sockaddr_in laddr, raddr;
	socklen_t rlen;
	char ip[IP_SIZE];

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if(sd < 0){
		perror("socket");
		exit(0);
	}

	int val=1;
	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,\
				&val, sizeof(val)) < 0){
		perror("setsockopt()");
		exit(0);
	}

	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(atoi(argv[2]));

	//if(inet_pton(AF_INET,"0.0.0.0",&laddr.sin_addr) != 1){
	if(inet_pton(AF_INET,argv[1],&laddr.sin_addr) != 1){
		perror("inet_pton()");
		exit(0);
	}

	if(bind(sd, (void *)&laddr, sizeof(laddr)) < 0){
		perror("bind()");
		exit(0);
	}

	listen(sd, 200);
	while (1) {
		newsd = accept(sd, (void *)&raddr, &rlen);

		if(newsd<0){
			if(errno == EAGAIN ||errno == EINTR){
				continue;
			}
			perror("accept()");
			exit(-2);
		}

		if (use_keepalive) {

			//ret = anetSetTcpNoDelay(newsd);
			//if (ret == ANET_ERR) {
			//	printf("TcpNoDelay error\n");
			//	exit(1);
			//}

			int interval=10;
			ret = anetKeepAlive(newsd, interval);
			if (ret == ANET_ERR) {
				printf("keppalive error\n");
				exit(1);
			}
		}

		if(inet_ntop(AF_INET, (void *)&raddr.sin_addr, \
					ip, IP_SIZE) == NULL){
			perror("inet_ntop()");
			exit(-2);
		}

		printf("radd:%s rport:%d\n", \
				ip, htons(raddr.sin_port));

		worker(newsd);
	}

	return 0;
}
