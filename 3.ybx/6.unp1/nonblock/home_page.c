#include "web.h"

void home_page(const char*host, int port, char *page, \
		struct file filearr, int *nfiles){
	int fd;
	fd = Tcp_connect(host , port);
	if (fd == -1) {
		return -1;



	return 0;
}

