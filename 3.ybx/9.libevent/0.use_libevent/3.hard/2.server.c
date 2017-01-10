#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <event.h>
#include <string.h>
#include <event.h>
#include <errno.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>

typedef struct sockaddr SA;




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

//一个客户端已经连接上server了
//当此函数调用,libvent已经帮accept了这个客户端
//客户端文件描述符为fd
void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
		struct sockaddr *scok, int socklen, void *arg)
{
	printf("accept client fd:%d\n", fd);
	struct event_base *base = (struct event_base*) arg;
	//为fd分配bufferevent
	struct bufferevent *bev = bufferevent_socket_new(base, fd,
			BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(bev, socket_read_cb, NULL, event_cb, NULL);
	bufferevent_enable(bev, EV_READ|EV_PERSIST);
}

int main(int argc, char *argv[])
{
	if (argc<2) {
		printf("./a.out 9999\n");
		exit(1);
	}

	//1.base
	struct event_base* base = event_base_new();

	//2.bind
	char *port = argv[1];
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(atoi(port));
	int backlog = 10;
	struct evconnlistener *listener = evconnlistener_new_bind(
			base, listener_cb, base, LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE,
			backlog, (struct sockaddr*)&sin, sizeof(struct sockaddr));

	event_base_dispatch(base);
	evconnlistener_free(listener);
	event_base_free(base);
	return 0;
}
