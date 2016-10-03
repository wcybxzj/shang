#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>


#define PATHNAME    "/tmp/myfifo3"

static void sig_handler(int s);
void open_nonblock()
{
    int fd = -1;
    char buf[BUFSIZ] = "";

	remove(PATHNAME);
    if (mkfifo(PATHNAME, 0644) < 0) {
        perror("mkfifo()");
        exit(1);
    }

    fd = open(PATHNAME,O_NONBLOCK|O_WRONLY);
	if (fd == -1) {
		if (errno == ENXIO) {
			printf("ENXIO\n");
			exit(1);
		}
	}
}

void read_nonblock()
{
    int fd = -1;
    char buf[BUFSIZ] = "";
	int len;
	pid_t pid;
	remove(PATHNAME);
    if (mkfifo(PATHNAME, 0644) < 0) {
        perror("mkfifo()");
        exit(1);
    }

	pid = fork();

	if (pid==0) {
		sleep(1);
		fd = open(PATHNAME,O_NONBLOCK|O_RDONLY);
		if (fd == -1) {
			if (errno == ENXIO) {
				printf("ENXIO\n");
				exit(1);
			}
		}
		len = read(fd, buf, BUFSIZ);
		if (len==-1) {
			if (errno == EAGAIN) {
				printf("EAGAIN\n");
				exit(-1);
			}
		}
	}else{
		fd = open(PATHNAME,O_WRONLY);
		sleep(3);
		wait(NULL);
	}
}

//1->2->3->4
//必须捕捉信号SIGPIPE
void write_nonblock()
{
    int fd = -1;
    char buf[BUFSIZ] = "abc";
	int len;
	pid_t pid;
	remove(PATHNAME);
    if (mkfifo(PATHNAME, 0644) < 0) {
        perror("mkfifo()");
        exit(1);
    }

	pid = fork();
	if (pid==0) {
		//1
		fd = open(PATHNAME,O_NONBLOCK|O_RDONLY);
		if (fd==-1) {
			perror("open");
		}
		sleep(2);
		//3
		close(fd);
	}else{
		signal(SIGPIPE, sig_handler);
		sleep(1);
		//2
		fd = open(PATHNAME,O_WRONLY|O_NONBLOCK);
		if (fd==-1) {
			perror("open");
		}
		sleep(3);
		//4
		len = write(fd, buf, BUFSIZ);
		if (len==-1) {
			if (errno == EPIPE) {
				printf("EPIPE\n");
			}
		}
		wait(NULL);
	}
}

//tlpi-44.9中 非阻塞open read write的假错的简单演示
int main (void)
{
	//open_nonblock();
	//read_nonblock();
	write_nonblock();
	return 0;
}

static void sig_handler(int s){
	printf("sig!!!%d\n", s);
}

