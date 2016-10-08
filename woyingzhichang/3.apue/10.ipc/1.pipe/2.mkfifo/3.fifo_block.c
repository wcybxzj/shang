#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>

#define SIZE 1024

#define PATHNAME    "/tmp/myfifo3"

void func(int sig){
	if (sig==13) {
		printf("SIGPIPE\n");
	}
}

//子进程open以写方式打开并且阻塞 ,open返回阻塞
//直到父进程open以读方式,子进程open才能解除阻塞
void open_block()
{
    int fd = -1;
	pid_t pid;
	remove(PATHNAME);
    if (mkfifo(PATHNAME, 0644) < 0) {
        perror("mkfifo()");
        exit(1);
    }
	pid = fork();
	if (pid == 0) {
		while (1) {
			printf("open(O_WRONLY) block\n");
			fd = open(PATHNAME,O_WRONLY);
			printf("open(O_WRONLY) unblock\n");
			if (fd == -1) {
				if (errno == ENXIO) {
					printf("open(O_WRONLY) ENXIO\n");
					sleep(1);
				}
			}else{
				printf("open(O_WRONLY) success\n");
				break;
			}
		}
		exit(0);
	}else{
		sleep(2);
		fd = open(PATHNAME,O_RDONLY);
		if (fd < 0) {
			perror("open(O_RDONLY) fail");
			exit(1);
		}
		wait(NULL);
	}
}






int main(){
	open_block();
	exit(0);
}
