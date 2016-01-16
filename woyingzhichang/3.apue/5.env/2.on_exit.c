#include <stdio.h>
#include <stdlib.h>

void fun(int status, void *arg){
	printf("exit status is %d\n", status);
	printf("args is %s\n", (char *) arg);
}

int main(int argc, const char *argv[])
{

	puts("begin");

	on_exit(fun,"001");
	on_exit(fun,"002");
	on_exit(fun,"003");

	exit(123);

	return 0;
}
