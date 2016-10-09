#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <libgen.h>
#include <errno.h>
#include <strings.h>

int main(int argc, char *argv[])
{
	int sd, ret;
	char *ip;
	int port;
	struct sockaddr_in addr;

	if (argc!=3) {
		printf("usage:%s ip port\n",
				basename(argv[0]));
		exit(1);
	}

	ip = argv[1];
	port = atoi(argv[2]);

	sd = socket(PF_INET, SOCK_STREAM, 0);
	if (sd <=0) {
		perror("socket()");
		exit(1);
	}

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &addr.sin_addr);

	ret = connect(sd, (struct sockaddr *) &addr, \
			  sizeof(addr));
	if (ret== -1) {
		perror("connect()");
		exit(1);
	}

	while (1) {
		printf("client sleep 1\n");
		sleep(1);
	}

	return 0;
}
