#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#define PATHNAME    "/tmp/myfifo3"

static void sig_handler(int s){
	if (s==13) {
		printf("SIGPIPE\n");
	}
}

//child: open(O_NONBLOCK|O_RDWR) success
void open_RDWR_nonblock()
{
    int fd = -1, i;
	pid_t pid;
	remove(PATHNAME);
    if (mkfifo(PATHNAME, 0644) < 0) {
        perror("mkfifo()");
        exit(1);
    }
	pid = fork();
	if (pid == 0) {
		while (1) {
			fd = open(PATHNAME,O_NONBLOCK|O_RDWR);
			if (fd == -1) {
				if (errno == ENXIO) {
					printf("child: open(O_NONBLOCK|O_RDWR) ENXIO\n");
					sleep(1);
				}
			}else{
				printf("child: open(O_NONBLOCK|O_RDWR) success\n");
				break;
			}
		}
		exit(0);
	}else{
		wait(NULL);
	}
}



//child: open(O_NONBLOCK|O_RDONLY) success
void open_RDONLY_nonblock()
{
    int fd = -1, i;
	pid_t pid;
	remove(PATHNAME);
    if (mkfifo(PATHNAME, 0644) < 0) {
        perror("mkfifo()");
        exit(1);
    }
	pid = fork();
	if (pid == 0) {
		while (1) {
			fd = open(PATHNAME,O_NONBLOCK|O_RDONLY);
			if (fd == -1) {
				if (errno == ENXIO) {
					printf("child: open(O_NONBLOCK|O_RDONLY) ENXIO\n");
					sleep(1);
				}
			}else{
				printf("child: open(O_NONBLOCK|O_RDONLY) success\n");
				break;
			}
		}
		exit(0);
	}else{
		wait(NULL);
	}
}

/*
child: open(O_NONBLOCK|O_WRONLY) ENXIO
child: open(O_NONBLOCK|O_WRONLY) ENXIO
parent: sleep 1
parent: sleep 1
child: open(O_NONBLOCK|O_WRONLY) ENXIO
parent: sleep 1
child: open(O_NONBLOCK|O_WRONLY) ENXIO
parent: sleep 1
child: open(O_NONBLOCK|O_WRONLY) ENXIO
parent: sleep 1
child: open(O_NONBLOCK|O_WRONLY) ENXIO
child: open(O_NONBLOCK|O_WRONLY) success
*/
//子进程open以写方式打开并且非阻塞 ,open返回ENXIO
//直到父进程open以读方式,子进程open才能成功
void open_WRONLY_nonblock()
{
    int fd = -1, i;
	pid_t pid;
	remove(PATHNAME);
    if (mkfifo(PATHNAME, 0644) < 0) {
        perror("mkfifo()");
        exit(1);
    }
	pid = fork();
	if (pid == 0) {
		while (1) {
			fd = open(PATHNAME,O_NONBLOCK|O_WRONLY);
			if (fd == -1) {
				if (errno == ENXIO) {
					printf("child: open(O_NONBLOCK|O_WRONLY) ENXIO\n");
					sleep(1);
				}
			}else{
				printf("child: open(O_NONBLOCK|O_WRONLY) success\n");
				break;
			}
		}
		exit(0);
	}else{
		for (i = 0; i < 5; i++) {
			sleep(1);
			printf("parent: sleep 1\n");
		}
		fd = open(PATHNAME,O_RDONLY);
		if (fd < 0) {
			perror("open(O_RDONLY) fail");
			exit(1);
		}
		wait(NULL);
	}
}

//情况1:
//父进程直接wait,FIFO中没内容并且FIFO的没有写者存在,子进程read EOF
//
//情况2:
//父进程 open(O_WRONLY),FIFO没有内容但是FIFO有写者,子进程read一直EAGAIN循环
void read_nonblock()
{
	int fd = -1;
	char buf[BUFSIZ];
	int len;
	pid_t pid;
	remove(PATHNAME);
	if (mkfifo(PATHNAME, 0644) < 0) {
		perror("mkfifo()");
		exit(1);
	}
	pid = fork();
	if (pid==0) {
		while (1) {
			fd = open(PATHNAME, O_NONBLOCK|O_RDONLY);
			if (fd == -1) {
				perror("open");
				exit(1);
			}else{
				break;
			}
		}

		while (1) {
			len = read(fd, buf, BUFSIZ);
			if (len==-1) {
				if (errno == EAGAIN) {
					printf("EAGAIN\n");
					sleep(1);
					continue;
				}else{
					perror("read");
					exit(1);
				}
			}else if(len == 0){
				printf("EOF\n");
				exit(1);
			}else if(len > 0){
				printf("read success %s\n", buf);
				exit(0);
			}
		}
		exit(0);
	}else{
		fd = open(PATHNAME,O_WRONLY);
		wait(NULL);
	}
}

/*
./4.fifo_nonblock 
ENXIO
ENXIO
write success len:8192
write success len:8192
SIGPIPE
EPIPE
SIGPIPE
EPIPE
*/
//子进程写打开时候,FIFO还没有读打开子进程open报错ENXIO
//父进程只读open,子进程write success
//父进程只读close(fd),子进程write时候FIFO没有人在读write引起SIGPIPE返回EPIPE
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
		signal(SIGPIPE, sig_handler);
		while (1) {
			fd = open(PATHNAME, O_NONBLOCK|O_WRONLY);
			if (fd==-1) {
				if (errno == ENXIO) {
					printf("ENXIO\n");
					sleep(1);
					continue;
				}else{
					perror("child open");
					exit(1);
				}
			}
			else{
				break;
			}
		}

		while (1) {
			len = write(fd, buf, BUFSIZ);
			if (len==-1) {
				if (errno == EPIPE) {
					printf("EPIPE\n");
					sleep(1);
					continue;
				}else{
					perror("write");
					exit(1);
				}
			}else{
				printf("write success len:%d\n", len);
				sleep(1);
			}
		}
		exit(0);
	}else{
		sleep(1);
		fd = open(PATHNAME,O_RDONLY);
		if (fd==-1) {
			perror("parent open");
			exit(1);
		}

		sleep(1);
		close(fd);
		wait(NULL);
	}
}

//tlpi-44.9中 非阻塞open read write的假错的简单演示
int main (void)
{
	//open_RDWR_nonblock();
	open_RDONLY_nonblock();
	//open_WRONLY_nonblock();
	//read_nonblock();
	//write_nonblock();
	return 0;
}


