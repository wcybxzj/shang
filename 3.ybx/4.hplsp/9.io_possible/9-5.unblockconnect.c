#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <strings.h>
/* According to POSIX.1-2001 */
#include <sys/select.h>
/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

int setnonblocking(int fd){
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl( fd, F_SETFL, new_option );
	return old_option;
}

int unblock_connect(const char*ip, int port, int time){
	int ret;
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	int fdopt = setnonblocking(sockfd);
	//connect
	ret = connect(sockfd, (struct sockaddr *) &address, sizeof(address));
	if (ret == 0) {
		printf("connect ret == 0\n");
		fcntl(sockfd, F_SETFL, fdopt);
		return sockfd;
	}else if(errno != EINPROGRESS){
		perror("connect()");
		return -1;
	}

	printf("errno is errno: EINPROGRESS\n");
	//select
	fd_set writefds;
	struct timeval timeout;
	FD_ZERO(&writefds);
	FD_SET(sockfd, &writefds);
	timeout.tv_sec = time;
	timeout.tv_usec = 0;
	ret = select(sockfd+1, NULL, &writefds, NULL, &timeout);
	if (ret <= 0) {
		perror("select");
		close(sockfd);
		return -1;
	}

	if (!FD_ISSET(sockfd, &writefds)) {
		printf("no events on sockfd found\n");
		close(sockfd);
		return -1;
	}

	//getsockopt
	int error = 0;
	socklen_t length = sizeof(error);
	if (getsockopt(sockfd,SOL_SOCKET, SO_ERROR, &error, &length ) < 0) {
		perror("getsockopt()");
		close(sockfd);
		return -1;
	}
	if (error != 0) {
		printf("getsockopt errno is:%d\n", error);
		close(sockfd);
		return -1;
	}
	fcntl(sockfd, F_SETFL, fdopt);
	//printf("success\n");
	return sockfd;
}

int main(int argc, const char *argv[])
{
	if (argc !=3) {
		printf("./a.out ip port\n");
		exit(1);
	}

	int ret, sockfd;
	const char *ip = argv[1];
	int port = atoi(argv[2]);

	sockfd = unblock_connect(ip, port ,10);
	if (sockfd < 0) {
		printf("unblock_connect sockfd < 0\n");
		return 1;
	}

	ret = send(sockfd, "abc", 3, 0);
	if (ret<0) {
		perror("send");
		exit(1);
	}

	pause();
	return 0;
}
