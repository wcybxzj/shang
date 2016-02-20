#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "mytbf.h"
#include <string.h>

//CPS: character per seconds 每秒允许的字节数
#define CPS 10
#define BUFSIZE 1024

//令牌上限
#define BURST 100

int main(int argc, char *argv[])
{
	int size;
	int num, ret, pos;
	int fd1, fd2=1;
	char str[BUFSIZE];
	mytbf_t *tbf;

	if (argc < 2) {
		fprintf(stderr, "lack of argv\n");
		exit(1);
	}

	tbf = mytbf_init(CPS, BURST);

	if (NULL == tbf) {
		fprintf(stderr, "mytbf_init failed\n");
		exit(1);
	}
	while ((fd1 = open(argv[1], O_RDONLY)) < 0) {
		if (errno == EINTR) {
			continue;
		}
		perror("open():");
		return -1;
	}

	while (1) {
		//按照BUFSIZE取token, 返回获取实际取到的token
		size = mytbf_fetchtoken(tbf, BUFSIZE);
		if (size < 0) {
			fprintf(stderr, "mytbf_fetchtoken failed! %s\n", strerror(-size));
			exit(1);
		}
		while ((num = read(fd1, str, size)) < 0) {
			if (errno == EINTR) {
				continue;
			}
			perror("read():");
			return -3;
		}
		if (num == 0) {
			break;
		}

		//如果获取token 10个,但是实际只read到3个字符,归还多余的7个
		if (size>num) {
			mytbf_returntoken(tbf, size-num);
		}


		//坚持写够num个字节，因为信号会打断阻塞的系统调用
		pos = 0;
		while (num > 0) {
			ret = write(fd2, str+pos, num);
			if (ret < 0) {
				if (errno == EINTR) {
					continue;
				}
				perror("write():");
				return -2;
			}
			num -= ret;
			pos += ret;
		}
	}
	close(fd1);
	mytbf_destroy(tbf);
	return 0;
}
