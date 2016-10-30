#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>

int main(int argc, char *argv[])
{
	int pipefd[2];
	int sd, newsd, ret;
	if (argc!=3) {
		printf("%s ip port, \n", basename(argv[0]));
		exit(1);
	}

	const char *ip = argv[1];
	int port = atoi(argv[2]);
	struct sockaddr_in address;

	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd<0) {
		perror("socket");
		exit(1);
	}

	ret = bind(sd, (struct sockaddr *) &address, sizeof(address));
	if (ret == -1) {
		perror("bind()");
		exit(1);
	}

	ret = listen(sd, 5);
	if (ret == -1) {
		perror("listen");
		exit(1);
	}

	struct sockaddr_in  client;
	socklen_t client_addrlen = sizeof(client);
	while (1) {
		newsd = accept(sd, (struct sockaddr*) &client, &client_addrlen);
		if (newsd < 0) {
			perror("accept");
			exit(1);
		}

		ret = pipe(pipefd);
		if (ret == -1) {
			perror("pipe");
			exit(1);
		}

		ret = splice(newsd, NULL, pipefd[1], NULL, \
				32678, SPLICE_F_MOVE|SPLICE_F_MORE);
		if (ret == -1) {
			perror("splice");
			exit(1);
		}
		ret = splice(pipefd[0], NULL, newsd, NULL, \
				32678, SPLICE_F_MOVE|SPLICE_F_MORE);
		if (ret == -1) {
			perror("splice");
			exit(1);
		}

		close(pipefd[0]);
		close(pipefd[1]);
		close(newsd);
	}
	close(sd);
	return 0;
}
