#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define SIZE 1024

//a.out &
//让程序在后台运行,但是直接成后台停止了,就算bg让它在后台运行也不行
//因为后台程序不能使用标准输入

int main(int argc, const char *argv[])
{
	char str[SIZE];
	FILE *fp;
	fp = fopen("/tmp/out", "w");
	if (NULL == fp) {
		perror("fopen():");
		exit(1);
	}

	while (fgets(str, SIZE, stdin)) {
		fprintf(fp,"%s",str);
		fflush(NULL);
	}
	return 0;
}
