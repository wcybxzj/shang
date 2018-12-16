#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

struct man{
	char name[10];
	int sex;
};

/*
输出:
parent: name:ybx, sex:1
child: name:ybx, sex:1  ----子进程继承父进程的堆空间
child: name:MM, sex:2
parent: name:ybx, sex:1 ----说明父子进程的堆空间是完全独立的
*/
int main(int argc, const char *argv[])
{
	pid_t pid;
	struct man *p = malloc(sizeof(struct man));
	strcpy(p->name, "ybx");
	p->sex=1;

	printf("parent: name:%s, sex:%d\n",p->name, p->sex);

	pid = fork();

	if (pid<0) {
		printf("error\n");
		exit(0);
	}else if(pid==0){
		printf("child: name:%s, sex:%d\n",p->name, p->sex);
		strcpy(p->name, "MM");
		p->sex=2;
		printf("child: name:%s, sex:%d\n",p->name, p->sex);
		exit(0);
	}else{
		wait(NULL);
	}

	printf("parent: name:%s, sex:%d\n",p->name, p->sex);

	return 0;
}
