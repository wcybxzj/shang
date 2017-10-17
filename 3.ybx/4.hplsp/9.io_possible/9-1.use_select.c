#include <sys/socket.h>
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

/*
 ./5-6.oobsend 127.0.0.1 5678
 send oob data out


 ./9-1.use_select 127.0.0.1 5678
 ip is 127.0.0.1 and port is 5678
 connect with ip: 127.0.0.1 and port: 44181
 select one
 get 3 bytes of normal data: 123
 select one
 get 2 bytes of normal data: ab
 select one
 get 1 bytes of oob data:c
 select one
 get 3 bytes of normal data: 123
 select one
 */
int main(int argc, char *argv[])
{
	if ( argc <= 2) {
		printf("useage: %s ip port\n", basename( argv[0] ) );
		return 1;
	}
	const char* ip = argv[1];
	int port = atoi( argv[2] );
	printf("ip is %s and port is %d\n", ip, port);

	int ret = 0;
	struct sockaddr_in address;
	bzero( &address, sizeof( address ) );
	address.sin_family = AF_INET;
	inet_pton( AF_INET, ip, &address.sin_addr );
	address.sin_port = htons( port );

	int listenfd = socket( PF_INET, SOCK_STREAM, 0 );
	assert( listenfd >= 0);
	ret = bind( listenfd, (struct sockaddr* )&address, sizeof( address ) );
	assert( ret != -1 );
	ret = listen( listenfd, 5 );
	assert( ret != -1);

	struct sockaddr_in client_address;
	socklen_t client_addrlength = sizeof( client_address );
	printf("block on accept\n");
	int connfd = accept( listenfd, ( struct sockaddr* )&client_address, &client_addrlength );
	printf("return from accept\n");
	if ( connfd < 0 ) {
		printf("errno is: %d\n", errno );
		close( listenfd );
	}

	char remote_addr[INET_ADDRSTRLEN];
	printf( "connect with ip: %s and port: %d\n", \
			inet_ntop( AF_INET, &client_address.sin_addr, remote_addr, INET_ADDRSTRLEN ), \
			ntohs( client_address.sin_port ) );

	char buf[1024];
	fd_set read_fds;
	fd_set exception_fds;
	FD_ZERO( &read_fds );
	FD_ZERO( &exception_fds );


	//源码这里有问题,必须注释掉
	//SO_OOBINLINE unp24.2-page509和unp24.3,这种叫"在线接受带外数据"
	//recv中MSG_OOB是这种叫"带外接受带外数据"
	//int so_oobinline = 1;
	//setsockopt( connfd, SOL_SOCKET, SO_OOBINLINE, &so_oobinline, sizeof( so_oobinline ) );
	while ( 1 ) {
		memset( buf, '\0', sizeof( buf ) );
		FD_SET( connfd, &read_fds );
		FD_SET( connfd, &exception_fds );
		ret = select( connfd + 1, &read_fds, NULL, &exception_fds, NULL);
		printf("select one\n");
		if ( ret < 0 ) {
			printf("selection failure\n");
			break;
		}
		if ( FD_ISSET( connfd, &read_fds ) ){
			ret = recv( connfd, buf, sizeof( buf )-1, 0 );
			if ( ret <= 0 ) {
				break;
			}
			printf("get %d bytes of normal data: %s\n", ret, buf);
		}else if ( FD_ISSET( connfd, &exception_fds ) ) {
			ret = recv( connfd, buf, sizeof( buf )-1, MSG_OOB );
			if ( ret <= 0 ) {
				break;
			}
			printf("get %d bytes of oob data:%s\n", ret, buf );
		}
	}
	close( connfd );
	close( listenfd );
	return 0;
}
