#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>          /* See NOTES */
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

void socket_read_cb(int fd, short events, void *arg)
{
	char msg[4096]={'\0',};
	struct event* ev = (struct event*) arg;
	int len = read(fd, msg, sizeof(msg)-1);
	if (len<0) {
		printf("read error\n");
		event_free(ev);
		close(fd);
		return;
	}else if(len == 0){
		printf("EOF\n");
		event_free(ev);
		close(fd);
		return;
	}else{
		printf("recv from client:%s\n", msg);
	}
	char reply_msg[2048]= "I have recieved the msg:";
	strcat(reply_msg+strlen(reply_msg), msg);
	write(fd, reply_msg, strlen(reply_msg));
}

void socket_read_cb2(int fd, short events, void *arg)
{
	char msg[4096]={'\0',};
	int len = read(fd, msg, sizeof(msg)-1);
	if (len<0) {
		printf("read error\n");
		close(fd);
		return;
	}else if(len == 0) {
		printf("EOF\n");
		close(fd);
		return;
	}else{
		printf("recv from client:%s\n", msg);
	}
	char reply_msg[2048]= "I have recieved the msg:";
	strcat(reply_msg+strlen(reply_msg), msg);
	write(fd, reply_msg, strlen(reply_msg));
}

void accept_cb(int fd, short events, void* arg){
	evutil_socket_t sockfd;
	struct sockaddr_in client;
	socklen_t len = sizeof(client);

	//1.accept
	sockfd = accept(fd, (struct sockaddr*) &client, &len);
	evutil_make_socket_nonblocking(sockfd);
	printf("accept a client %d\n", sockfd);

	//2.event_new
	struct event_base *base = (struct event_base*) arg;

	//写法1: event_new = malloc event+event_assign
	struct event *ev = event_new(NULL, -1, 0, NULL, NULL);
	event_assign(ev, base, sockfd, EV_READ|EV_PERSIST,
			socket_read_cb, (void*)ev);
	event_add(ev, NULL);

	//写法2:
	//struct event *ev = event_new(base, sockfd,EV_READ|EV_PERSIST,
	//		socket_read_cb2, NULL);
	//event_add(ev, NULL);
}

int main(int argc, char *argv[])
{
	if (argc<2) {
		printf("./a.out 9999\n");
		exit(1);
	}

	char *port = argv[1];
	//socket+bind+listen
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
	return 0;
}
