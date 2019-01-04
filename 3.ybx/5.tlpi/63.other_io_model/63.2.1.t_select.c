#include <stdio.h>
#include <stdlib.h>
/* According to POSIX.1-2001 */
#include <sys/select.h>
/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void errExit(char *str){
	perror(str);
	exit(1);
}


//目的:证明任何fd必须先将向布置的fd放入集合,才能在select返回后有机会去判断事件
//============================================================================
//mkfio 123
/*
测试1:不对STDIN_FILENO进行select布置监视任务
终端1:
/63.2.1.t_select 123
open fd:3
此数据是输入到标准输入的,但是因为没布置监视任务所以select不会返回
0:read not ok
1:read not ok
2:read not ok
3:read ok

终端2:
cat > 123
pipe data
*/
//============================================================================
/*
测试2:对STDIN_FILENO进行select布置监视任务
终端1:
./63.2.1.t_select 123
此数据是输入到标准输入的,这次监视了标准输入所以可以从select返回
open fd:3
0:read ok
1:read not ok
2:read not ok
3:read not ok

终端2:
cat > 123
*/
int main(int argc, const char *argv[])
{
	if (argc < 2 || strcmp(argv[1], "--help") == 0){
		printf("%s file...\n", argv[0]);
		exit(1);
	}

	int j, ready, fd;
	int nfds = 0;
	fd_set readfds;
	FD_ZERO(&readfds);

	//测试2
	FD_SET(STDIN_FILENO, &readfds);
	nfds++;

	for (j = 1; j < argc; j++) {
		fd = open(argv[j], O_RDONLY);
		printf("open fd:%d\n", fd);
		if (fd == -1)
			errExit("open");

		if (fd >= FD_SETSIZE) {
			printf("fd >= FD_SETSIZE\n");
			exit(1);
		}

		if (fd >= nfds) {
			nfds = fd+1;
		}
		FD_SET(fd, &readfds);
	}

	ready = select(nfds, &readfds, NULL, NULL, NULL);
	if (ready ==-1) {
		perror("select");
		exit(1);
	}

	for (fd = 0; fd < nfds ; fd++) {
		printf("%d:%s\n", fd, FD_ISSET(fd, &readfds)? "read ok":"read not ok");
	}
	return 0;
}
