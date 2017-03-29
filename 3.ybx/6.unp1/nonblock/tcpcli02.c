#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, const char *argv[])
{
	int sockfd;
	struct sockaddr_in servaddr;
	if (argc!=2) {
		printf("./a.out ip\n");
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(7);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	
	connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr) );

	str_cli(stdin, sockfd);

	return 0;
}
