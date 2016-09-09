#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

#define PATHNAME    "/tmp/myfifo3"

//用程序演示命名管道阻塞读
//再开一个终端:echo 111 > /tmp/myfifo3
int main (void)
{
    pid_t pid;
    int fd = -1;
    char buf[BUFSIZ] = "";

	remove(PATHNAME);
    if (mkfifo(PATHNAME, 0644) < 0) {
        perror("mkfifo()");
        exit(1);
    }

    fd = open(PATHNAME, O_RDWR);
	printf("block\n");
    read(fd, buf, BUFSIZ);
	printf("unblock\n");
    printf("%s\n", buf);

    return 0;
}
