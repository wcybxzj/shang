#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

//CPS: character per seconds 每秒允许的字节数
#define CPS 10
#define SIZE CPS

//令牌上限
#define BURST 100

static volatile int token = 0;

void alrm_handler(int s)
{
	alarm(1);
	token++;
	if (token > BURST) {
		token = BURST;
	}
}

int main(int argc, char *argv[])
{
	int count = 0;
	int num, ret, pos;
	int fd1, fd2=1;
	char str[SIZE];

	if (argc < 2) {
		fprintf(stderr, "lack of argv\n");
		exit(1);
	}

	signal(SIGALRM, alrm_handler);
	alarm(1);

	while ((fd1 = open(argv[1], O_RDONLY)) < 0) {
		if (errno == EINTR) {
			continue;
		}
		perror("open():");
		return -1;
	}

	while (1) {
		//忙等待
		//while (!token);

		//不需要忙等待
		while (!token)
			pause();

		token--;
		count++;
		while ((num = read(fd1, str, SIZE)) < 0) {
			if (errno == EINTR) {
				continue;
			}
			perror("read():");
			return -3;
		}
		if (num == 0) {
			break;
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
		sleep(1);
	}
	printf("循环次数 %d\n", count);
	close(fd1);
	return 0;
}
