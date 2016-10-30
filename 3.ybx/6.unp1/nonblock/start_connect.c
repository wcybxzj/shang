#include "web.h"


#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

void start_connect(struct file* fptr){
	struct addrinfo *ai;
	int fd, flags, ret;
	ai = host_serv(fptr->f_host, fptr->f_port, 0, SOCK_STREAM);
	if (ai == NULL) {
		perror("host_serv");
		exit(1);
	}

	fd = socket(ai->ai_family, ai->ai_socktype, \
			ai->ai_protocol);
	if (fd == -1) {
		perror("socket");
		exit(1);
	}
	fptr->f_fd = fd;
	printf("start_connect for %s fd:%d\n", fptr->f_name, fd);

	flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags|O_NONBLOCK);

	ret = connect(fd, ai->ai_addr, ai->ai_addrlen);
	if (ret < 0 ) {
		if (errno!=EINPROGRESS) {
			perror("connect");
			exit(1);
		}
		printf("EINPROGRESS fd:%d\n", fd);
		fptr->f_flags = F_CONNECTING;
		FD_SET(fd, &rset);
		FD_SET(fd, &wset);

		if (fd > maxfd) {
			maxfd = fd;
		}
	}else if(ret >= 0){
		write_get_cmd(fptr);
	}
}
