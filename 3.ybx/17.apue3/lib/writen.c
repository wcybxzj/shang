#include "apue.h"

ssize_t writen(int fd, const void*ptr, size_t n){
	size_t nleft;
	ssize_t nwritten;

	nleft = n;
	while (nleft > 0) {
		nwritten = write(fd, ptr, nleft);
		if(nwritten < 0 ){/*出错*/
			if (nleft == n) {/*1个字节都没写进去*/
				return -1;
			}else{
				/* 之前成功写了一些数据*/
				/* 打断while 函数返回成功写入的数量*/
				break;
			}
		}else if(nwritten==0){/*fd关闭*/
			break;
		}else if(nwritten>0){/*处理没完全写完的情况*/
			nleft -= nwritten;
			ptr  += nwritten;
		}
	}

	return n-nleft;
}
