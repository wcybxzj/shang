#include "web.h"

void write_get_cmd(struct file *ptr){
	int n;
	char line[MAXLINE];
	n = snprintf(line, sizeof(line), GET_CMD, ptr->f_name);
	if (n<=0) {
		perror("snprintf");
		exit(1);
	}
	
	Writen(ptr->f_fd, line, n);

	FD_SET(ptr->f_fd, &rset);
	ptr->f_flags = F_READING;
	//printf(">>>>>>>>>>>%d\n", ptr->f_flags);
	if (ptr->f_fd > maxfd) {
		maxfd = ptr->f_fd;
	}
	printf("request %d bytes for %s, fd:%d\n", n, ptr->f_name, ptr->f_fd);
}

