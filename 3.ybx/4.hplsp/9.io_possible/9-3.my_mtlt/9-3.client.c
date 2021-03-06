#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <libgen.h>

int main(int argc, char *argv[])
{
	if ( argc != 4 ) {
		printf("usage: %s ip port forever\n",basename( argv[0] ) );
		return 1;
	}

	int i;
	const char* ip = argv[1];
	int port = atoi( argv[2] );
	char *forever = argv[3];

	struct sockaddr_in address;
	bzero( &address, sizeof( address ) );
	address.sin_family = AF_INET;
	address.sin_port = htons( port );
	inet_pton( AF_INET, ip, &address.sin_addr );
	int ret = 0;
	int fd= socket( PF_INET, SOCK_STREAM, 0 );

	ret = connect( fd, ( struct sockaddr* )&address, sizeof( address ) );
	if (ret==-1) {
		perror("connect");
		exit(-1);
	}

	//目的是测试服务端accept后,recv会如何返回
	//结果在测试这里中看到recv监控的是非阻塞io,一直在recv 返回错EAGAIN
	//for (i = 0; i < 10; i++) {
	//	printf("after connect sleep 1\n");
	//	sleep(1);
	//}

	char *str = "1234567890abcdefhijklm";
	write(fd, str, strlen(str));
	if (strcmp(forever,"forever")==0) {
		for (i = 0; i < 100; i++) {
			sleep(1);
			printf("sleep 1\n");
		}
	}
	return 0;
}
