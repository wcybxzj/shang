#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{
	//创建空洞文件
	//od -t c bigfile.txt
	int fd;
	fd = open("bigfile.txt", O_WRONLY|O_CREAT|O_TRUNC, 0666);
	if (-1 == fd) {
		perror("open()");
		exit(1);
	}
	//避免数据溢出 要在每个数字加单位
	//off_t size = 5LL*1024LL*1024LL*1024LL*1024LL-1LL;
	off_t size = 1024LL*1024LL*1024LL* 5LL-1LL;
	off_t ret;
	ftruncate(fd, size);
	return 0;
}
