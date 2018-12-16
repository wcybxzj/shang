#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

struct man {
	char name[10];
	int sex;
};

struct book{
	char name[10];
};

struct book book_global = {
	.name = "book1",
};

/*
输出:
parent: name:ybx, sex:1
child: name:ybx, sex:1  ----子进程继承父进程的堆空间
child: name:MM, sex:2
parent: name:ybx, sex:1 ----说明父子进程的堆空间是完全独立的
*/
int heap_test()
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

/*
parent: name:book1
child: name:book1 ----子进程继承父进程的全局变量
child: name:book2
parent: name:book1 ----说明父子进程的全局变量是完全独立的
*/
int globalvar_test()
{
	pid_t pid;

	printf("parent: name:%s\n", book_global.name);

	pid = fork();

	if (pid<0) {
		printf("error\n");
		exit(0);
	}else if(pid==0){
		printf("child: name:%s\n", book_global.name);
		strcpy(book_global.name, "book2");
		printf("child: name:%s\n", book_global.name);
		exit(0);
	}else{
		wait(NULL);
	}

	printf("parent: name:%s\n", book_global.name);

	return 0;
}

int main(int argc, const char *argv[])
{
	//heap_test();
	globalvar_test();
}
