#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 10
char* is_block;
int setnonblocking( int fd ){
	int old_option = fcntl( fd, F_GETFL );
	int new_option = old_option | O_NONBLOCK;
	fcntl( fd, F_SETFL, new_option );
	return old_option;
}
void addfd( int epollfd, int fd, int enable_et ){
	epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN;
	if ( enable_et ) {
		event.events |= EPOLLET;
	}
	epoll_ctl( epollfd, EPOLL_CTL_ADD, fd, &event );
	if ( strncmp( is_block, "unblock" ,5) == 0 ) {
		setnonblocking( fd );
	}
}
void lt( epoll_event* events, int number ,int epollfd, int listenfd ){
	char buf[ BUFFER_SIZE ];
	printf("lt number %d\n", number);
	for (int i = 0; i < number; i++) {
		int sockfd = events[i].data.fd;
		if ( sockfd == listenfd ) {
			printf("sockfd == listenfd \n");
			struct sockaddr_in client_address;
			socklen_t client_addrlength = sizeof( client_address );
			int connfd = accept( listenfd, ( struct sockaddr* )&client_address, &client_addrlength );
			addfd( epollfd, connfd, 0 );
		}else if ( events[i].events & EPOLLIN ) {
			printf("lt event trigger once\n");
			memset( buf, '\0', BUFFER_SIZE );
			int ret = recv( sockfd, buf, BUFFER_SIZE-1, 0 );
			if ( ret <= 0 ) {
				close( sockfd );
				printf("ret <=0 close sockfd \n");
				continue;
			}
			printf( "get %d bytes of content: %s\n", ret, buf );
		}else{
			printf("something else happened \n");
		}
	}
}
void et( epoll_event* events, int number,int epollfd , int listenfd ){
	char buf[ BUFFER_SIZE ];
	printf("et number %d\n", number);
	for ( int i = 0; i < number; i++ ) {
		int sockfd = events[i].data.fd;
		if ( sockfd == listenfd ) {
			printf("sockfd == listenfd \n");
			struct sockaddr_in client_address;
			socklen_t client_addrlength = sizeof( client_address );
			int connfd = accept( listenfd, ( struct sockaddr* )&client_address, &client_addrlength );
			addfd( epollfd, connfd, 1 );
		}else if ( events[i].events & EPOLLIN ) {
			printf("et event trigger once\n");
			while ( 1 ) {
				memset( buf, '\0', BUFFER_SIZE );
				printf("et block recv!!!!!\n");
				int ret = recv( sockfd, buf, BUFFER_SIZE-1, 0 );
				printf("et unblock recv!!!!!\n");
				if ( ret < 0 ) {
					if ( ( errno == EAGAIN ) || (errno == EWOULDBLOCK ) ) {
						printf(">>>>EAGAIN<<<\n");
						break;
					}
					close( sockfd );
					printf("ret < 0 close sockfd \n");
					break;
				}else if ( ret == 0 ) {
					close( sockfd );
					printf("ret == 0 close sockfd \n");
				}else {
					printf("get %d bytes of content: %s\n", ret, buf );
				}
			}
		}else {
			printf("something else happened \n");
		}
	}
}
int main(int argc, char *argv[])
{
	if ( argc <= 3 ) {
		printf("usage: %s ip port  is_et\n",basename( argv[0] ) );
		return 1;
	}
	const char* ip = argv[1];
	int port = atoi( argv[2] );
	char* is_et = argv[3];
	is_block = argv[4];
	struct sockaddr_in address;
	bzero( &address, sizeof( address ) );
	address.sin_family = AF_INET;
	address.sin_port = htons( port );
	inet_pton( AF_INET, ip, &address.sin_addr );
	int ret = 0;
	int listenfd = socket( PF_INET, SOCK_STREAM, 0 );
	assert( listenfd >= 0 );
    int reuse = 1;
    setsockopt( listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof( reuse ) );
	ret = bind( listenfd, ( struct sockaddr* )&address, sizeof( address ) );
	assert( ret != -1 );
	ret = listen( listenfd, 5 );
	assert( ret != -1 );

	epoll_event events[ MAX_EVENT_NUMBER ];
	int epollfd = epoll_create( 5 );
	assert( epollfd != -1 );
	addfd( epollfd, listenfd, 1 );
	while ( 1 ) {
		printf("blocked in epoll_wait\n");
		int ret = epoll_wait( epollfd, events, MAX_EVENT_NUMBER, -1 );
		printf("unblocked in epoll_wait\n");
		if ( ret < 0) {
			printf("epoll failure\n");
			break;
		}
		if ( strncmp( is_et,"et" ,2) == 0 ) {
			et( events, ret, epollfd, listenfd );
		}else {
			lt( events, ret, epollfd, listenfd );
		}
	}
	close( listenfd );
	return 0;
}
