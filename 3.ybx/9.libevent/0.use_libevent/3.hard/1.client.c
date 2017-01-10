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
	}else if(event & BEV_EVENT_CONNECTED){
		printf("the client conneted\n");
		return;
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

	//1.event_base
	struct event_base* base = event_base_new();

	//2.bev
	struct bufferevent* bev = bufferevent_socket_new(base, -1,
			BEV_OPT_CLOSE_ON_FREE);

	//3.STDIN_FILENO event
	struct event* ev_cmd = event_new(base, STDIN_FILENO,
			EV_READ|EV_PERSIST, cmd_msg_cb, (void*)bev);
	event_add(ev_cmd, NULL);

	//4.connect
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
	bufferevent_socket_connect(bev, (struct sockaddr *) &server_addr,
			sizeof(server_addr));

	//5.setcb
	bufferevent_setcb(bev, server_msg_cb, NULL, event_cb, (void *)ev_cmd);
	bufferevent_enable(bev, EV_READ|EV_PERSIST);

	//6.loop
	event_base_dispatch(base);

	printf("finished\n");
	return 0;
}
