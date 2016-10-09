#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <assert.h>
#include <libgen.h>
#include <strings.h>

#define bool int
#define true 0x1
#define false 0x0

bool stop = false;

static void sig_handler(int s){
	stop = true;
}

int main(int argc, const char *argv[])
{
	struct sockaddr_in address;
	struct sigaction sa;
	int sd, ret;
	const char *ip;
	int port, backlog;
	if (argc!=4) {
		printf("usage:%s ip port backlog\n");
		exit(1);
	}

	ip = argv[1];
	port = atoi(argv[2]);
	backlog = atoi(argv[3]);

	sa.sa_handler= sig_handler;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGTERM);
	sa.sa_flags = 0;
	sigaction(SIGTERM, &sa, NULL);

	sd = socket(PF_INET, SOCK_STREAM, 0);
	assert(sd>=0);

	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	inet_pton(AF_INET, ip, &address.sin_addr);

	ret = bind(sd, (struct sockaddr*)&address, sizeof(address));
	assert(ret>=0);

	ret = listen(sd, backlog);
	assert(ret>=0);

	while (!stop) {
		printf("sleep 1\n");
		sleep(1);
	}
	close(sd);
	return 0;
}
