#include "unp.h"

//ok >=0
//fail -1
ssize_t readn(int fd, void *vptr, size_t n){
	size_t nleft;
	size_t nread;
	nleft = n;

	while (nleft > 0) {
		nread = read(fd, vptr, nleft);
		if (nread < 0) {
			if (errno == EINTR) {
				continue;
			}else{
				return -1;
			}
		}else if (nread == 0) {
			break;
		}
		nleft -= nread;
		vptr += nread;
	}
	
	return n-nleft; // >=0
}

ssize_t Readn(int fd,  void *ptr, size_t n){
	ssize_t num;
	num = readn(fd, ptr, n);
	if (num < 0) {
		err_sys("readn error");
	}
	return num;
}


