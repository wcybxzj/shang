#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//如果不为SIGIO设置信号相应函数默认是终止程序

//kill -SIGIO 8703
int main(int argc, const char *argv[])
{
	int i;
	printf("pid:%d\n",getpid());
	for (i = 0; i < 100; i++) {
		printf("i:%d\n", i);
		sleep(1);
	}

	return 0;
}
