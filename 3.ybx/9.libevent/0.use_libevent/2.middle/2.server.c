#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <event.h>
#include <string.h>
#include <event.h>
#include <errno.h>

typedef struct sockaddr SA;
int  tcp_server_init(char *port, int listen_num)
{
	int errno_save;
	evutil_socket_t listenfd;

	//1.socket
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1) {
		return -1;
	}
	evutil_make_listen_socket_reuseable(listenfd);

	//2.bind
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;//任何ip自适应
	sin.sin_port = htons(atoi(port));
	if (bind(listenfd, (SA*)&sin, sizeof(sin)) < 0) {
		goto error;
	}

	//3.listen
	if (listen(listenfd, listen_num)< 0) {
		goto error;
	}
	evutil_make_socket_nonblocking(listenfd);

	return listenfd;

	error:
		errno_save = errno;
		evutil_closesocket(listenfd);
		errno = errno_save;
		return -1;
}

void socket_read_cb(struct bufferevent* bev, void* arg)
{
	char msg[4096]={'\0',};
	int len = bufferevent_read(bev, msg, sizeof(msg)-1);
	printf("recv from client:%s\n", msg);
	char reply_msg[2048]= "I have recieved the msg:";
	strcat(reply_msg+strlen(reply_msg), msg);
	bufferevent_write(bev, reply_msg, strlen(reply_msg));
}

void event_cb(struct bufferevent* bev, short event, void *arg)
{
	if (event & BEV_EVENT_EOF) {
		printf("connection close\n");
	}else if(event & BEV_EVENT_ERROR){
		printf("some other error\n");
	}
	bufferevent_free(bev);
}

void accept_cb(int fd, short events, void* arg){
	evutil_socket_t sockfd;
	struct sockaddr_in client;
	socklen_t len = sizeof(client);

	//1.accept
	sockfd = accept(fd, (struct sockaddr*) &client, &len);
	evutil_make_socket_nonblocking(sockfd);
	printf("accept a client %d\n", sockfd);

	//2.base
	struct event_base *base = (struct event_base*) arg;

	//3.bufferevent
	struct bufferevent *bev = bufferevent_socket_new(base, sockfd,
													BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(bev, socket_read_cb, NULL, event_cb, arg);
	bufferevent_enable(bev, EV_READ|EV_PERSIST);
}

int main(int argc, char *argv[])
{
	if (argc<2) {
		printf("./a.out 9999\n");
		exit(1);
	}

	char *port = argv[1];
	int listenfd = tcp_server_init(port, 10);
	if (listenfd == -1) {
		perror("tcp_server_init error");
		exit(1);
	}

	struct event_base* base = event_base_new();

	struct event *ev_listen = event_new(base, listenfd,
			EV_READ|EV_PERSIST, accept_cb, base);

	event_add(ev_listen, NULL);
	event_base_dispatch(base);
	event_base_free(base);
	return 0;
}
