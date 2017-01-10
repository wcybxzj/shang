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
#include <event2/buffer.h>
#include <event2/bufferevent.h>

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

void server_msg_cb(struct bufferevent* bev, void* arg)
{
	char msg[1024]={'\0'};
	size_t ret = bufferevent_read(bev, msg, sizeof(msg));
	printf("recv from server:%s",msg);
}

void cmd_msg_cb(int fd, short events, void*arg)
{
	int ret;
	char msg[1024]={'\0',};
	int sockfd = *((int *)arg);
	struct bufferevent* bev = (struct bufferevent*) arg;
	ret = read(fd, msg, sizeof(msg)-1);
	if (ret<0) {
		perror("read error");
		exit(1);
	}else if(ret == 0){
		printf("EOF\n");
		return;
	}else{
		bufferevent_write(bev, msg, ret);
	}
}

void event_cb(struct bufferevent* bev, short event, void* arg)
{
	if (event & BEV_EVENT_EOF) {
		printf("connection closed\n");
	}else if(event & BEV_EVENT_ERROR){
		printf("some other error\n");
	}

	bufferevent_free(bev);

	struct event *ev = (struct event *)arg;
	event_free(ev);
}

int main(int argc, char *argv[])
{
	if (argc<3) {
		printf("./a.out 127.0.0.1 9999\n");
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

	//3.sockfd bev
	struct bufferevent* bev = bufferevent_socket_new(base, sockfd,
			BEV_OPT_CLOSE_ON_FREE);

	//4.STDIN_FILENO event
	struct event* ev_cmd = event_new(base, STDIN_FILENO,
			EV_READ|EV_PERSIST, cmd_msg_cb, (void*)bev);
	event_add(ev_cmd, NULL);

	//5.setcb normal-fun and callback-fun
	bufferevent_setcb(bev, server_msg_cb, NULL, event_cb, (void *)ev_cmd);
	bufferevent_enable(bev, EV_READ|EV_PERSIST);

	//6.loop
	event_base_dispatch(base);

	printf("finished\n");
	return 0;
}
