#include "unp.h"


void Close(int fd){
	if (close(fd) == -1) {
		err_sys("close error");
	}
}
