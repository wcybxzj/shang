#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <libgen.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#define BUF_SIZE 1024

int connfd;

void sig_urg(int s){
	int ret;
	char buffer[ BUF_SIZE ];
	memset( buffer,'\0' ,BUF_SIZE-1 );
	ret = recv( connfd, buffer, BUF_SIZE-1, MSG_OOB );
	printf("got %d bytes of oob data '%s'\n", ret, buffer);
}


/*
终端1:
./5-7.oobrecv 127.0.0.1 1234
got 1 bytes of oob data 'c'
got 0 bytes of normal data '123ab'
got 0 bytes of normal data '123'
received EOF

终端2:
./5-6.oobsend 127.0.0.1 1234 
*/
//原来代码有问题，增加signal和fcnl信号驱动IO 才成功
int main(int argc, char *argv[])
{
	if ( argc <= 2 ) {
		printf("usage: %s ip port\n", basename( argv[0]) );
		return 1;
	}
	const char* ip = argv[1];
	int port = atoi( argv[2] );

	struct sockaddr_in address;
	bzero( &address, sizeof( address ) );
	address.sin_family = AF_INET;
	inet_pton( AF_INET ,ip ,&address.sin_addr);
	address.sin_port = htons( port );

	int sock = socket( PF_INET, SOCK_STREAM, 0 );
	assert( sock >= 0 );

	int val=1;
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,\
				&val, sizeof(val)) < 0){ 
		perror("setsockopt()");
		exit(0);
	}   

	int ret = bind( sock, ( struct sockaddr* )&address, sizeof( address ) );
	assert( sock != -1 );

	ret = listen( sock, 5 );
	assert( ret != -1 );

	struct sockaddr_in client;
	socklen_t client_addrlength = sizeof( client );
	connfd = accept(sock, ( struct sockaddr* )&client, &client_addrlength );

	signal(SIGURG, sig_urg);
	fcntl(connfd, F_SETOWN, getpid());

	if (connfd < 0) {
		printf( "errno is %d\n", errno );
	}else {
		char buffer[ BUF_SIZE ];
		for ( ; ; ) {
			memset( buffer,'\0' ,BUF_SIZE-1 );
		    if ( (ret = recv(connfd, buffer, BUF_SIZE - 1, 0)) == 0) {
		        printf("received EOF\n");
		        exit(0);
		    }
			printf("got %d bytes of normal data '%s'\n", ret, buffer);
		}

	}

	//获取本地和远程的ip addr和port
	unsigned int len;
	struct sockaddr_in local_address, remote_address;
	len = sizeof( local_address );
	getsockname( connfd, ( struct sockaddr* )&local_address, &len );
	printf( "Local IP address is: %s\n", inet_ntoa( local_address.sin_addr ) );
	printf("Local port is: %d\n", (int) ntohs(local_address.sin_port));

	getpeername( connfd, ( struct sockaddr* )&remote_address, &len );
	printf( "Local IP address is: %s\n", inet_ntoa( remote_address.sin_addr ) );
	printf("Local port is: %d\n", (int) ntohs(remote_address.sin_port));

	close( connfd );
	return 0;
}
