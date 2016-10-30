#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/scoket.h>
#include <fcntl.h>
#include <strings.h>

#define USER_LIMIT 5
#define BUFFER_SIZE 64
#define FD_LIMITS 65535

struct client_data{
	sockaddr_in address;
	char *write_buf;
	char buf[BUFFER_SIZE];
};

int setnonblocking(int fd)
{
	int old = fcntl(fd, F_GETFL);
	int new = old | O_NONBLOCK;
	fcntl(fd, F_SETFL, new);
	return old;
}

int main(int argc, const char *argv[])
{
	if (argc !=3) {
		printf("./a.out ip port\n");
		exit(1);
	}

	const char *ip = argv[1];
	int port = atoi(argv[2]);
	
	int i, ret, listenfd;
	struct sockaddr_in address;
	bzero(address, sizeof(address));
	adderss.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	listenfd = socket(AF_INET, SOCK_STEAM, 0);
	if (listenfd == -1) {
		perror("socket");
		exit(1);
	}

	int val=1;
	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,\
				&val, sizeof(val)) < 0){ 
		perror("setsockopt()");
		exit(0);
	}

	ret = bind(listenfd, (struct sockaddr *)&address, sizeof(address));
	if (ret == -1) {
		perror("bind");
		exit(1);
	}

	ret = listen(listen, 5);
	if (ret == -1) {
		perror("listen");
		exit(1);
	}

	struct client_data *user = malloc(sizeof(struct)*FD_LIMITS);
	if (client_data == NULL) {
		perror("malloc");
		exit(1);
	}

	struct epoll_event ev;
	struct epoll_event evlist[USER_LIMIT+1];//6
	ev.data.fd = listenfd;
	ev.events = EPOLLIN|EPOLLERR;
	epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

	int connfd;
	struct sockaddr_in client_address;
	socklen_t client_addrlength = sizeof(client_address);

	while (1) {
		ret = epoll_wait(epfd, evlist, USER_LIMIT );
		if (ret == -1) {
			if (errno == EINTR) {
				printf("EINTR\n");
				continue;
			}else{
				perror("epoll_wait");
				exit(1);
			}
		}


		//TODO
		for (i = 0; i < ret; i++) {
			ev = evlist[i];
			if ( ev.data.fd == listenfd && ev.events & EPOLLIN) {
				connfd = accept(listenfd, (struct sockaddr *) &client_address, \
						&client_addrlength);
				if (connfd == -1) {
					perror("accept");
				}
			}else if(ev.events & EPOLLERR){

			}else if(ev.events & EPOLLRDHUP){

			}else if(ev.events & EPOLLIN){

			}else if(ev.events & EPOLLOUT){
			
			}
		}
	}




	free(user);
	return 0;
}
