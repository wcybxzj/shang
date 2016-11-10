#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <strings.h>
#include <libgen.h>

int setnonblocking( int fd ){
    int old_option = fcntl( fd, F_GETFL );
    int new_option = old_option | O_NONBLOCK;
    fcntl( fd, F_SETFL, new_option );
    return old_option;
}

//如果想要出现超时只能在client把fd设置成nonblocking
int main(int argc, char *argv[])
{
	if (argc != 4) {
		printf("usage: %s ip port time\n", basename(argv[0]));
		exit(1);
	}

	char *ip = argv[1];
	int port = atoi(argv[2]);
	int time = atoi(argv[3]);

	int ret;
	int sockfd;
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);
	socklen_t addr_len = sizeof(address);

	struct timeval timeout;
	timeout.tv_sec = time;
	timeout.tv_usec = 0;
	socklen_t len = sizeof(timeout);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd ==-1) {
		perror("socket");
		exit(1);
	}

	//setnonblocking(sockfd);

	ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, len);
	if (ret == -1) {
		perror("setsockopt");
		exit(1);
	}

	ret = connect(sockfd, (struct sockaddr *)&address,  addr_len);
	if (ret == -1) {
		if (errno == EINPROGRESS) {
			printf("connect timeout\n");
			exit(1);
		}
		printf("%s\n", strerror(errno));
		exit(1);
	}

	int i;
	for (i = 0; i < 100; i++) {
		sleep(1);
		printf("sleep 1\n");
	}

	return 0;
}
