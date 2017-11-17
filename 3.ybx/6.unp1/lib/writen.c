#include "unp.h"

//ok    >=0
//fail  -1
ssize_t writen(int fd, const void *vptr ,size_t n){
	size_t nwritten, nleft;
	nleft = n;
	while (nleft > 0) {
		printf("writen():Begin Writen %d\n", nleft);
		nwritten = write(fd, vptr, nleft);
		if (nwritten <= 0) {
			if (errno == EINTR) {
				printf("writen():EINTR\n");
				continue;
			}else{
				return -1;
			}
		}
		vptr += nwritten;
		nleft -= nwritten;
		printf("writen():Already write %d, left %d, errno=%d\n", nwritten, nleft, errno);
	}
	return n;
}


void Writen(int fd, const void *ptr ,size_t n){
	if (writen(fd, ptr, n) != n) {
		err_sys("writen error");
	}
}
