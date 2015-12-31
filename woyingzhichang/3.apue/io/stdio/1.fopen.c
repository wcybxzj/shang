#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(int argc, const char *argv[])
{
	char *str;
	FILE * fp;
	fp = fopen("tmp", "r");//只有r和r+不会创建文件,才会在下面报错
	if (NULL == fp) {
		fprintf(stderr, "fopen():failed %d\n", errno);
		perror("fopen():");
		str = strerror(errno);
		fprintf(stderr, "fopen():%s\n", str);
		exit(1);
	}
	puts("ok");
	fclose(fp);
	exit(0);
}
