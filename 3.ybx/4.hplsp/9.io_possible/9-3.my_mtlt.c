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

void addfd( int epollfd, int fd, int enable_et ,int nonblock){
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN;
	printf("fd:%d\n", fd);
	if ( enable_et == ET_MODE) {
		printf("ET模式\n");
		event.events |= EPOLLET;
	}else{
		printf("LT模式\n");
	}
	epoll_ctl( epollfd, EPOLL_CTL_ADD, fd, &event );
	if ( nonblock == NONBLOCK ) {
		printf("非阻塞模式\n");
		setnonblocking( fd );
	}else{
		printf("阻塞模式\n");
	}
}

void et(struct epoll_event *events, int num, \
		int epollfd, int listenfd, int is_block){
	int i, ret;
	char buf[BUFFER_SIZE];
	int sockfd, connfd;
	struct sockaddr_in client_address;
	socklen_t client_addrlength = sizeof( client_address);
	for (i = 0; i < num; i++) {
		sockfd = events[i].data.fd;
		if (sockfd == listenfd) {
			printf("sockfd == listenfd\n");
			connfd = accept(listenfd, \
					(struct sockaddr *) &client_address, \
					&client_addrlength);
			addfd(epollfd, connfd ,ET_MODE, is_block);
		}else if(events[i].events & EPOLLIN){
			printf("event事件触发一次\n");
			while (1) {
				memset(buf, '\0', BUFFER_SIZE);
				printf("recv block\n");
				ret = recv(sockfd, buf, BUFFER_SIZE-1, 0);
				printf("recv unblock\n");
				if (ret < 0) {
					if (errno == EAGAIN) {
						printf(">>EAGAIN<<\n");
						printf("sleep 1\n");
						sleep(1);
						break;
					}else{
						epoll_ctl( epollfd, EPOLL_CTL_DEL, sockfd, NULL);
						close(sockfd);
						printf("ret < 0\n");
						break;
					}
				}else if(ret == 0){
					epoll_ctl( epollfd, EPOLL_CTL_DEL, sockfd, NULL);
					close(sockfd);
					printf("ret ==  0\n");
				}else{
					printf("get %d bytes of content: %s\n", ret, buf);
				}
			}
		}else{
			printf("其他事件发生\n");
            printf("  fd=%d; events: %s%s%s\n", events[i].data.fd,
                    (events[i].events & EPOLLIN)  ? "EPOLLIN "  : "", 
                    (events[i].events & EPOLLHUP) ? "EPOLLHUP " : "", 
                    (events[i].events & EPOLLERR) ? "EPOLLERR " : "");
		}
	}
}

void et_func(int listenfd, int is_block){
	struct epoll_event events[ MAX_EVENT_NUMBER ];
	int epollfd = epoll_create( 5 );
	assert( epollfd != -1 );
	addfd( epollfd, listenfd, ET_MODE, BLOCK);

	while ( 1 ) {
		printf("blocked in epoll_wait\n");
		int ret = epoll_wait( epollfd, events, MAX_EVENT_NUMBER, -1 );
		printf("unblocked in epoll_wait\n");
		if ( ret < 0) {
			printf("epoll failure\n");
			break;
		}
		et( events, ret, epollfd, listenfd ,is_block);
	}
	close( listenfd );
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

	/*测试2:et + io nonblock ,可能要多次测试才能出EAGAIN*/
	//et_func(listenfd, NONBLOCK);

	/*测试3:et + io block 成功*/
	//et_func(listenfd, BLOCK);

	//比较测试2和测试3:
	//测试2中使用io nonblock 比 测试3中io block的好处
	//当客户端不结束链接而且也不发数据的时候
	//测试2还能响应其他请求阻塞在epoll_wait 
	//测试3阻塞在recv其他fd如果来事件根本没机会相应
	//资料:tlpi63.4.4在边缘触发中应该避免使用阻塞IO造成其他文件描述符饥饿的情况
	return 0;
}

	/*测试2的数据 client  noforver*/
	/*
	./9-3.my_mtlt 127.0.0.1 1234
	fd:3
	ET模式
	阻塞模式
	blocked in epoll_wait
	unblocked in epoll_wait
	sockfd == listenfd
	fd:5
	ET模式
	非阻塞模式
	blocked in epoll_wait
	unblocked in epoll_wait
	event事件触发一次
	recv block
	recv unblock
	get 9 bytes of content: 123456789
	recv block
	recv unblock
	get 9 bytes of content: 0abcdefhi
	recv block
	recv unblock
	get 4 bytes of content: jklm
	recv block
	recv unblock
	ret ==  0
	recv block
	recv unblock
	ret < 0
	blocked in epoll_wait

	终端2:
	./9-3.client 127.0.0.1 1234 noforver
	*/

	//-------------------------------------------------
	//-------------------------------------------------

	/*测试2的数据 client  forver*/
	/*
	终端1:
	./9-3.my_mtlt 127.0.0.1 1234
	fd:3
	ET模式
	阻塞模式
	blocked in epoll_wait
	unblocked in epoll_wait
	sockfd == listenfd
	fd:5
	ET模式
	非阻塞模式
	blocked in epoll_wait
	unblocked in epoll_wait
	event事件触发一次
	recv block
	recv unblock
	get 9 bytes of content: 123456789
	recv block
	recv unblock
	get 9 bytes of content: 0abcdefhi
	recv block
	recv unblock
	get 4 bytes of content: jklm
	recv block
	recv unblock
	>>EAGAIN<<
	sleep 1
	blocked in epoll_wait( 还能相应其他请求 )
	unblocked in epoll_wait
	event事件触发一次(client结束sleep, ctrl+c触发)
	recv block
	recv unblock
	ret ==  0
	recv block
	recv unblock
	ret < 0
	blocked in epoll_wait

	终端2:
	./9-3.client 127.0.0.1 1234 forever
	sleep 1
	sleep 1
	sleep 1
	sleep 1
	sleep 1
	sleep 1
	sleep 1
	ctrl+c在my_mtlt触发第二个事件
	*/

	//---------------------------------------------------
	//---------------------------------------------------
	//---------------------------------------------------

	/*测试3的数据 client  noforver*/
	/*
	./9-3.my_mtlt 127.0.0.1 1234
	fd:3
	ET模式
	阻塞模式
	blocked in epoll_wait
	unblocked in epoll_wait
	sockfd == listenfd
	fd:5
	ET模式
	阻塞模式
	blocked in epoll_wait
	unblocked in epoll_wait
	event事件触发一次
	recv block
	recv unblock
	get 9 bytes of content: 123456789
	recv block
	recv unblock
	get 9 bytes of content: 0abcdefhi
	recv block
	recv unblock
	get 4 bytes of content: jklm
	recv block
	recv unblock
	ret ==  0
	recv block
	recv unblock
	ret < 0
	blocked in epoll_wait
	^C

	终端2:
	./9-3.client 127.0.0.1 1234 forever
	*/

	/*测试3的数据 client  forver*/
	/*
	./9-3.my_mtlt 127.0.0.1 1234
	fd:3
	ET模式
	阻塞模式
	blocked in epoll_wait
	unblocked in epoll_wait
	sockfd == listenfd
	fd:5
	ET模式
	阻塞模式
	blocked in epoll_wait
	unblocked in epoll_wait
	event事件触发一次
	recv block
	recv unblock
	get 9 bytes of content: 123456789
	recv block
	recv unblock
	get 9 bytes of content: 0abcdefhi
	recv block
	recv unblock
	get 4 bytes of content: jklm
	recv block(client sleep时候阻塞)


	recv unblock(client ctrl+c, recv解除阻塞)
	ret ==  0
	recv block
	recv unblock
	ret < 0
	blocked in epoll_wait

	终端2:
	./9-3.client 127.0.0.1 1234 forever
	sleep 1
	sleep 1
	sleep 1
	sleep 1
	sleep 1
	sleep 1
	sleep 1
	ctrl+c
	*/
