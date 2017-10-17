#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/epoll.h>
#include <string.h>

#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 10


#define ET_MODE 1
#define NONBLOCK 1
#define BLOCK 0

int setnonblocking( int fd ){
	int old_option = fcntl( fd, F_GETFL );
	int new_option = old_option | O_NONBLOCK;
	fcntl( fd, F_SETFL, new_option );
	return old_option;
}


/*
终端1:
./9-3.my_mtlt 127.0.0.1 1234
block recv!!!!!
unblock recv!!!!!
ret < 0 EAGAIN read later
sleep 1
block recv!!!!!
unblock recv!!!!!
get 9 bytes of content: 123456789
block recv!!!!!
unblock recv!!!!!
get 9 bytes of content: 0abcdefhi
block recv!!!!!
unblock recv!!!!!
get 4 bytes of content: jklm
block recv!!!!!
unblock recv!!!!!
ret < 0 EAGAIN read later
sleep 1
block recv!!!!!
unblock recv!!!!!
ret < 0 EAGAIN read later
...........略 无限..............

终端2:
./9-3.client 127.0.0.1 1234 forver
*/

/*
终端1:
./9-3.my_mtlt 127.0.0.1 1234
block recv!!!!!
unblock recv!!!!!
ret < 0 EAGAIN read later
sleep 1
block recv!!!!!
unblock recv!!!!!
get 9 bytes of content: 123456789
block recv!!!!!
unblock recv!!!!!
get 9 bytes of content: 0abcdefhi
block recv!!!!!
unblock recv!!!!!
get 4 bytes of content: jklm
block recv!!!!!
unblock recv!!!!!
ret == 0 close  connfd
block recv!!!!!
unblock recv!!!!!
ret < 0 close  connfd

终端2:
./9-3.client 127.0.0.1 1234 noforver
*/
void normal_nonblock(int listenfd){
	struct sockaddr_in client_address;
	socklen_t client_addrlength = sizeof( client_address );
	int connfd = accept( listenfd, \
			( struct sockaddr* )&client_address, \
			&client_addrlength );
	setnonblocking(connfd);
	char buf[BUFFER_SIZE];
	while (1) {
		memset( buf, '\0', BUFFER_SIZE );
		printf("block recv!!!!!\n");
		int ret = recv( connfd, buf, BUFFER_SIZE-1, 0 );
		printf("unblock recv!!!!!\n");
		if ( ret < 0 ) {
			if ( ( errno == EAGAIN ) || (errno == EWOULDBLOCK ) ) {
				printf("ret < 0 EAGAIN read later\n");
				sleep(1);
				printf("sleep 1\n");
				continue;
			}
			close( connfd );
			printf("ret < 0 close  connfd\n");
			break;
		}else if ( ret == 0 ) {
			close( connfd );
			printf("ret == 0 close  connfd\n");
		}else {
			printf("get %d bytes of content: %s\n", ret, buf );
		}
	}
}

int main(int argc, char *argv[])
{
	if ( argc != 3 ) {
		printf("usage: %s ip port\n",basename( argv[0] ) );
		return 1;
	}

	int listenfd;
	const char* ip = argv[1];
	int port = atoi( argv[2] );
	struct sockaddr_in address;
	bzero( &address, sizeof( address ) );
	address.sin_family = AF_INET;
	address.sin_port = htons( port );
	inet_pton( AF_INET, ip, &address.sin_addr );
	int ret = 0;
	listenfd = socket( PF_INET, SOCK_STREAM, 0 );
    int reuse = 1;
    setsockopt( listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof( reuse ) );
	ret = bind( listenfd, ( struct sockaddr* )&address, sizeof( address ) );
	ret = listen( listenfd, 5 );

	/*测试1:*/
	normal_nonblock(listenfd);

	return 0;
}
