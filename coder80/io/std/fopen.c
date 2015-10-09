#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main()
{
	FILE *fp;

	fp = fopen("tmp","w");
	if(fp == NULL)
	{
//		printf("errno = %d\n",errno);
//		perror("fopen()");
		fprintf(stderr,"fopen():%s\n",strerror(errno));
		exit(1);
	}

	puts("ok!");

	fclose(fp);

	exit(0);
}


