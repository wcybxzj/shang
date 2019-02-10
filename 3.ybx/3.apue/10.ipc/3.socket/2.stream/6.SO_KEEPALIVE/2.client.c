#include "header.h"

void worker(int newsd)
{
	int ret;
	char buf[1000]={0};
	while (1) {
		ret = read(newsd, buf, sizeof(buf));
		if (ret==0) {
			printf("server close\n");
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
int main(int argc, char *argv[])
{
	if (argc <= 3) {
		printf("usage: %s ip port yes\n", basename( argv[0] ) );
	}
	const char* ip = argv[1];
	int port = atoi( argv[2] );

	int use_keepalive=0;

	if (strcmp(argv[3],"yes")==0) {
		printf("use keep-alive\n");
		use_keepalive=1;
	}else{
		printf("not use keep_alive\n");
	}

	struct sockaddr_in server_address;
	bzero( &server_address, sizeof( server_address ) );
	server_address.sin_family = AF_INET;
	inet_pton( AF_INET, ip, &server_address.sin_addr );
	server_address.sin_port = htons( port );

	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if ( sockfd < 0 ){
		printf("socket() error\n");
		exit(1);
	}

	if (use_keepalive) {
		//int val=1;
		//if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,\
		//			&val, sizeof(val)) < 0){
		//	perror("setsockopt()");
		//	exit(0);
		//}

		int interval=3;
		int ret = anetKeepAlive(sockfd, interval);
		if (ret == ANET_ERR) {
			printf("keppalive error\n");
			exit(1);
		}
	}

	if ( connect( sockfd, ( struct sockaddr* )&server_address, sizeof( server_address) ) < 0 ) {
		printf("connection failed\n");
	}else {
		worker(sockfd);
		printf("sleep 200\n");
		sleep(200);
		printf("sleep finish\n");
	}

	close( sockfd );
	return 0;
}
