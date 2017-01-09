#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <event.h>
#include <event2/util.h>

typedef struct sockaddr SA;

int tcp_connect(char *ip, char *port)
{
	int ret, sockfd, save_errno;
	struct sockaddr_in server_addr;

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(port));
	ret = inet_pton(AF_INET, ip, &server_addr.sin_addr);
	if (ret == 0) {//ip invaild
		errno = EINVAL;
		return;
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		return;
	}

	ret = connect(sockfd, (SA*)&server_addr,
			sizeof(server_addr));
	if (ret == -1) {
		save_errno = errno;//close may be error
		close(sockfd);
		errno = save_errno;
		return ;
	}
	evutil_make_socket_nonblocking(sockfd);
	return sockfd;
}

void socket_read_cb(int fd, short events, void*arg)
{
	int ret;
	char msg[1024]={'\0',};
	ret = read(fd, msg, sizeof(msg)-1);
	printf("ret:%d\n", ret);
	if (ret<0) {
		perror("read error");
		exit(1);
	}else if(ret == 0){
		printf("EOF, server close client\n");
		exit(1);
	}else{
		printf("%s", msg);
	}
	memset(msg, '\0', sizeof(msg));
}

void cmd_msg_cb(int fd, short events, void*arg)
{
	int ret;
	char msg[1024]={'\0',};
	int sockfd = *((int *)arg);

	ret = read(fd, msg, sizeof(msg)-1);

	if (ret<0) {
		perror("read error");
		exit(1);
	}else if(ret == 0){
		printf("EOF\n");
		return;
	}else{
		write(sockfd, msg, ret);
	}
	memset(msg, '\0', sizeof(msg));
}

int main(int argc, char *argv[])
{
	if (argc<3) {
		printf("./a.out 127.0.0.1 1234\n");
		exit(1);
	}

	//1.connect
	int sockfd = tcp_connect(argv[1],argv[2]);
	if (sockfd == -1) {
		perror("socket error");
		return -1;
	}
	printf("connect successful\n");

	//2.event_base
	struct event_base* base = event_base_new();

	//3.create sockfd event
	struct event *ev_sockfd = event_new(base, sockfd,
			EV_READ|EV_PERSIST,
			socket_read_cb, NULL);
	event_add(ev_sockfd, NULL);

	//4.create STDIN_FILENO event
	struct event* ev_cmd = event_new(base, STDIN_FILENO,
			EV_READ|EV_PERSIST, cmd_msg_cb, (void*)&sockfd);
	event_add(ev_cmd, NULL);

	//5.loop
	event_base_dispatch(base);

	printf("finished\n");
	return 0;
}
