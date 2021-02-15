#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

void sys_err(const char *str)
{
	perror(str);
	exit(1);
}

void func1()
{
	int a =1;
	printf("%d\n", a);
}


int main(int argc, char *argv[])
{
	func1();
	return 0;
}
