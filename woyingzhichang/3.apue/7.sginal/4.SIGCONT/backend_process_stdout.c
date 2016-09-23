#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define SIZE 1024
//./a.out &
//标准输出一直打印a, 证明后台运行的程序可以使用stdout
int main(int argc, const char *argv[])
{
	while (1) {
		printf("a\n");
		sleep(2);
	}
	return 0;
}
