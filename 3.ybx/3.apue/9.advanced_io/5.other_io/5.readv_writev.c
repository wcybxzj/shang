#include <stdio.h>
#include <sys/uio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

//ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
//ssize_t writev(int fd, const struct iovec *iov, int iovcnt);

//struct iovec {
//    void  *iov_base;    /* Starting address */
//    size_t iov_len;     /* Number of bytes to transfer */
//};

//测试:
//123
//vim /tmp/iov.txt
int main(void){
	int num;
	struct iovec iov1_arr[2];
	char *buf1 = calloc(3, sizeof(char));
	char *buf2 = calloc(3, sizeof(char));
	iov1_arr[0].iov_base = buf1;
	iov1_arr[0].iov_len = 3; 
	iov1_arr[1].iov_base = buf2;
	iov1_arr[1].iov_len = 3; 
	num = readv(0, iov1_arr, 2);
	printf("readv num is %d\n", num);

	int fd;
	fd = open("/tmp/iov.txt", O_RDWR|O_CREAT);
	if (fd < 0) {
		perror("open():");
		exit(1);
	}
	iov1_arr[0].iov_len = strlen(iov1_arr[0].iov_base); 
	iov1_arr[1].iov_len = strlen(iov1_arr[1].iov_base); 
	num = writev(fd, iov1_arr, 2);
	printf("writev num is %d\n", num);
}
