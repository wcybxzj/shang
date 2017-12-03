#include "open.h"
#include <fcntl.h>

#define BUFFSIZE 8192

int main(int argc, const char *argv[])
{
	int fd, n, ret;
	char line[MAXLINE];
	/*获取想要打开的文件名*/
	while (fgets(line, MAXLINE, stdin)!=NULL) {
		/*处理最后的换行符*/
		if (line[strlen(line)-1] == '\n') {
			line[strlen(line)-1] = 0;
		}

		/*获取一个fd从子进程的服务中*/
		fd = csopen(line, O_RDONLY);
		if (fd<0) {
			continue;
		}

		/*从fd读取*/
		while ( (n =read(fd, line, MAXLINE)) > 0 ) {
			ret = write(STDOUT_FILENO, line, n);
			if (ret!=n) {
				perror("write()");
				exit(1);
			}
		}

		/*read出错则报错*/
		if (n<0) {
			printf("err sys\n");
			exit(0);
		}

		close(fd);
	}

	return 0;
}
