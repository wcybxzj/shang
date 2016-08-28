#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define NAME "num.txt"

int main(void){
	printf("current pid is %d\n", getpid());
	int fd, num, ret, ret2;
	fd = open(NAME, O_RDONLY);
	if(fd <0 ){
		perror("open():");
		exit(-1);
	}

	//下面此对测试毫无影响
	//说明其他进程close(fd); 不会异常关闭1.lockf中上好的锁
	close(fd);
	fd = open(NAME, O_RDONLY);
	if(fd <0 ){
		perror("open():");
		exit(-1);
	}

	ret = lockf(fd, F_TEST, 0);
	if (ret>0) {
		printf("ret > 0\n");
	}else if(ret ==0){
		printf("ret == 0\n");
		ret2 = lockf(fd, F_LOCK, 0);
		if (ret2 == 0) {
			printf("file is locked by current process\n");
		}
		lockf(fd, F_ULOCK, 0);
	}else{
		if (errno == EACCES) {
			printf("file is locked by another process\n");
			exit(0);
		}
		perror("lockf():");
		exit(-1);
	}

	close(fd);
	exit(0);
}
