#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "mytbf.h"
#include <string.h>
#include <unistd.h>

//CPS: character per seconds 每秒允许的字节数
#define CPS 10
#define BUFSIZE 1024
#define TID_NUM 3
//令牌上限
#define BURST 100

typedef struct _myst {
	int i;
	char *filename;
	int cps;
	int burst;
} myst;

myst* arr[TID_NUM];

static void* func(void *p){
	myst *me = p;
	char of[20] = {0,};
    snprintf(of, sizeof(of), "file%i.txt", me->i);
	//printf("%d\n", me->i);
	//printf("%s\n", me->filename);
	//printf("%s\n", of);

	int size;
	int num, ret, pos;
	int fd1, fd2;
	char str[BUFSIZE];
	mytbf_t *tbf;

	fd2 = open(of, O_WRONLY|O_CREAT|O_TRUNC,0644);

	tbf = mytbf_init(me->cps, me->burst);

	if (NULL == tbf) {
		fprintf(stderr, "mytbf_init failed\n");
		pthread_exit(NULL);
	}
	while ((fd1 = open(me->filename, O_RDONLY)) < 0) {
		if (errno == EINTR) {
			continue;
		}
		perror("open():");
		pthread_exit(NULL);
	}

	while (1) {
		//按照BUFSIZE取token, 返回获取实际取到的token
		size = mytbf_fetchtoken(tbf, BUFSIZE);
		if (size < 0) {
			fprintf(stderr, "mytbf_fetchtoken failed! %s\n", strerror(-size));
			pthread_exit(NULL);
		}
		while ((num = read(fd1, str, size)) < 0) {
			if (errno == EINTR) {
				continue;
			}
			perror("read():");
			pthread_exit(NULL);
		}
		if (num == 0) {
			break;
		}

		//如果获取token 10个,但是实际只read到3个字符,归还多余的7个
		if (size>num) {
			mytbf_returntoken(tbf, size-num);
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
				pthread_exit(NULL);
			}
			num -= ret;
			pos += ret;
		}
	}
	close(fd1);
	mytbf_destroy(tbf);

	return NULL;
}

int main(int argc, char *argv[])
{
	printf("current pid is: %d\n",getpid());

	if (argc < 2) {
		fprintf(stderr, "lack of argv\n");
		exit(1);
	}

	int i;
	myst* me;
	pthread_t tid_arr[TID_NUM];
	myst *myst_arr[TID_NUM];
	for (i = 0; i < TID_NUM; i++) {
		me = malloc(sizeof(myst));
		me->i = i;
		me->filename = argv[1];
		me->cps=i+1;
		me->burst=100;
		myst_arr[i] = me;
		pthread_create(tid_arr+i, NULL, func, myst_arr[i]);
	}

	for (i = 0; i < TID_NUM; i++) {
		pthread_join(tid_arr[i], NULL);
	}

	return 0;
}
