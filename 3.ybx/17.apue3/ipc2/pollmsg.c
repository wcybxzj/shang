#include "apue.h"
#include <poll.h>
#include <pthread.h>
#include <sys/msg.h>
#include <sys/socket.h>

#define NQ 3
#define MAXMSZ 512
#define KEY 0x123

struct threadinfo {
	int qid;
	int fd;
};

struct mymesg {
	long mtype;
	char mtext[MAXMSZ];
};


void* helper(void *arg)
{
	int n, ret;
	struct mymesg m;
	struct threadinfo *tip= (struct threadinfo *)arg;
	while (1) {
		memset(&m, 0, sizeof(m));
		/*MSG_NOERROR 意思是如果数据大于 缓冲尺寸会被截断 而不报错*/
		n = msgrcv(tip->qid, &m, MAXMSZ, 0, MSG_NOERROR);
		if (n<0) {
			err_sys("msgrcv()");
		}
		ret = write(tip->fd, m.mtext, n);
		if (n < 0) {
			err_sys("write error");
		}
	}
}

/*核心:线程+socketpair来解决XSI IPC无法使用IO多路复用的问题*/
int main(int argc, const char *argv[])
{
	int i, n ,ret, err;
	int fd[2];
	int qid[NQ];
	struct pollfd pfd[NQ];
	struct threadinfo ti[NQ];
	pthread_t tid[NQ];
	char buf[MAXMSZ];

	for (i = 0; i < NQ; i++) {
		qid[i]=msgget((KEY+i), IPC_CREAT|0666);
		if (qid[i]<0) {
			err_sys("msgget() error");
		}
		printf("queue index:%d, msgid:%d\n",i, qid[i]);

		ret = socketpair(AF_UNIX, SOCK_DGRAM, 0, fd);
		if (ret<0) {
			err_sys("socketpair() error");
		}

		pfd[i].fd = fd[0];
		pfd[i].events = POLLIN;
		ti[i].qid = qid[i];
		ti[i].fd = fd[1];
		/*这里这种创建线程的传参存在隐患*/
		err = pthread_create(&tid[i], NULL, helper, &ti[i]);
		if (err!=0) {
			err_exit(err, "pthread_create() error");
		}
	}

	while (1) {
		ret = poll(pfd, NQ, -1);
		if (ret <0) {
			err_sys("poll() error");
		}
		for (i = 0; i < NQ; i++) {
			if (pfd[i].revents & POLLIN) {
				n = read(pfd[i].fd, buf, sizeof(buf));
				if (n<0) {
					err_sys("read() error");
				}else if(n == 0){
					err_sys("socketpair 关闭");
				}else{
					/*buf[n]=0;*/
					printf("queue id:%d, message:%s\n",
							qid[i], buf);
				}
			}
		}
	}
	return 0;
}
