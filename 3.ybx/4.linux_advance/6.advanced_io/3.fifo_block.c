#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#define SIZE 1024

#define PATHNAME    "/tmp/myfifo3"

static void sig_handler(int s){
	if (s==13) {
		printf("SIGPIPE\n");
	}
}

//child: open(O_RDWR) success
void open_RDWR_block()
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
			fd = open(PATHNAME,O_RDWR);
			if (fd == -1) {
				if (errno == ENXIO) {
					printf(" ENXIO\n");
					sleep(1);
				}
			}else{
				printf("child: open(O_RDWR) success\n");
				break;
			}
		}
		exit(0);
	}else{
		wait(NULL);
	}
}

/*
child: open(O_RDONLY) block
parent: sleep 1
parent: sleep 1
child: open(O_RDONLY) unblock
child: open(O_RDONLY) success
*/
void open_RDONLY_block()
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
			printf("child: open(O_RDONLY) block\n");
			fd = open(PATHNAME,O_RDONLY);
			printf("child: open(O_RDONLY) unblock\n");
			if (fd == -1) {
				if (errno == ENXIO) {
					printf("open(O_RDONLY) ENXIO\n");
					sleep(1);
				}
			}else{
				printf("child: open(O_RDONLY) success\n");
				break;
			}
		}
		exit(0);
	}else{
		for (i = 0; i < 2; i++) {
			sleep(1);
			printf("parent: sleep 1\n");
		}
		fd = open(PATHNAME,O_WRONLY);
		if (fd < 0) {
			perror("open(O_RDONLY) fail");
			exit(1);
		}
		wait(NULL);
	}
}

/*
child: open(O_WRONLY) block
parent: sleep 1
parent: sleep 1
child: open(O_WRONLY) unblock
child: open(O_WRONLY) success
*/
void open_WRONLY_block()
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
			printf("child: open(O_WRONLY) block\n");
			fd = open(PATHNAME,O_WRONLY);
			printf("child: open(O_WRONLY) unblock\n");
			if (fd == -1) {
				if (errno == ENXIO) {
					printf("open(O_WRONLY) ENXIO\n");
					sleep(1);
				}
			}else{
				printf("child: open(O_WRONLY) success\n");
				break;
			}
		}
		exit(0);
	}else{
		for (i = 0; i < 2; i++) {
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

/*
child: open block
parent: sleep 1
parent: sleep 1
parent: sleep 1
parent: sleep 1
child: open unblock
child: read block
parent: sleep 1
parent: sleep 1
child: read unblock
child:read success abc
child: read block
parent:sleep 1
child: read unblock
child:read success def
child: read block
parent:sleep 1
child: read unblock
child:EOF
*/
//第1个3秒中 父进程什么都没做,	子进程只读open阻塞
//第1个3秒后 父进程进行只写open,子进程只读open成功
//第2个3秒中 父进程什么也不做,	子进程read还是阻塞
//第2个3秒后 父进程write,		子进程read读取成功
//第7秒后	 父进程write		子进程read读取成功
//第8秒后	 父进程close(fd)	子进程read到EOF
void read_block()
{
	int fd = -1;
	char buf[BUFSIZ];
	int len, i;
	pid_t pid;
	remove(PATHNAME);
	if (mkfifo(PATHNAME, 0644) < 0) {
		perror("mkfifo()");
		exit(1);
	}
	pid = fork();
	if (pid==0) {
		while (1) {
			printf("child: open block\n");
			fd = open(PATHNAME,O_RDONLY);
			printf("child: open unblock\n");
			if (fd == -1) {
				perror("open");
				exit(1);
			}else{
				break;
			}
		}
		while (1) {
			printf("child: read block\n");
			len = read(fd, buf, BUFSIZ);
			printf("child: read unblock\n");
			if (len==-1) {
				perror("read");
				exit(1);
			}else if(len == 0){
				printf("child:EOF\n");
				exit(1);
			}else if(len > 0){
				printf("child:read success %s\n", buf);
			}
		}
		exit(0);
	}else{
		for (i = 0; i < 3; i++) {
			printf("parent: sleep 1\n");
			sleep(1);
		}
		fd = open(PATHNAME,O_WRONLY);
		for (i = 0; i < 3; i++) {
			printf("parent: sleep 1\n");
			sleep(1);
		}
		write(fd, "abc", strlen("abc"));
		sleep(1);
		printf("parent:sleep 1\n");
		write(fd, "def", strlen("def"));
		sleep(1);
		printf("parent:sleep 1\n");
		close(fd);
		wait(NULL);
	}
}

/*
child: open(O_WRONLY) block
parent sleep 1
parent sleep 1
child: open(O_WRONLY) unblock
write success len:8192
parent sleep 1
write success len:8192
parent sleep 1
write success len:8192
SIGPIPE
child:EPIPE
SIGPIPE
child:EPIPE
*/
void write_block()
{
	int fd = -1, i;
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
			printf("child: open(O_WRONLY) block\n");
			fd = open(PATHNAME,O_WRONLY);
			printf("child: open(O_WRONLY) unblock\n");
			if (fd==-1) {
				perror("child: open");
				exit(1);
			}
			else{
				break;
			}
		}

		while (1) {
			len = write(fd, buf, BUFSIZ);
			if (len==-1) {
				if (errno == EPIPE) {
					printf("child:EPIPE\n");
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
		for (i = 0; i < 2; i++) {
			sleep(1);
			printf("parent sleep 1\n");
		}
		fd = open(PATHNAME,O_RDONLY);
		if (fd==-1) {
			perror("parent:open");
			exit(1);
		}
		for (i = 0; i < 2; i++) {
			sleep(1);
			printf("parent sleep 1\n");
		}
		close(fd);
		wait(NULL);
	}
}

int main(){
	//open_RDWR_block();
	open_RDONLY_block();
	//open_WRONLY_block();
	//read_block();
	//write_block();
	exit(0);
}
