#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>

#define BUFFER_SIZE 10

int main(int argc, char *argv[])
{
	if (argc!=3) {
		printf("usage:./%s ip port\n", basename(argv[0]));
		exit(1);
	}

	char *ip = argv[1];
	int port = atoi(argv[2]);

	char buf[BUFFER_SIZE];
	int i, ret;
	int listenfd;
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	ret = connect(listenfd, (struct sockaddr *)&address, sizeof(address) );
	if (ret == -1) {
		perror("connect");
		exit(1);
	}

	for (i = 0; i < 20; i++) {
		bzero(buf, BUFFER_SIZE);
		snprintf(buf, BUFFER_SIZE-1, "%d,",i);
		//printf("%s\n", buf);
		write(listenfd, buf, strlen(buf));
		sleep(2);
	}

	close(listenfd);
	return 0;
}
