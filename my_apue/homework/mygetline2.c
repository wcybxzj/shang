#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define START_SIZE 5
#define INC_SIZE 1
//1.malloc 不要重复申清
//2.每行结尾都有\n,无论下面有没有内容,哪怕文件最后一行也有\n 然后才是EOF
//3.fgets 如果存储10个内容包含换行，必须申请11个空间 man 3 fgets
ssize_t mygetline(char **lineptr, size_t *n, FILE *stream)
{
	int num = 0, c, i = 0, back_num = 0;
	int pos;

	if (*lineptr == NULL) {
		*n = START_SIZE;
		*lineptr = malloc(*n);
		if (NULL == *lineptr) {
			return -1;
		}
		memset(*lineptr, 0x00, *n);
	}

	while (1) {
		c = getc(stream);
		if (c=='\n') {
			num++;
			break;
		}
		if (c==EOF) {
			printf("EOF\n");
			return -2;
		}
		num++;
	}

	while (num >= *n) {
		*n +=INC_SIZE;
		*lineptr = realloc(*lineptr, *n);
		if(NULL == *lineptr) {
			return -20;
		}
	}

	fseek(stream, -num, SEEK_CUR);
	fgets(*lineptr, *n, stream);
	return num;
}

int main(int argc,char **argv)
{
	int ret;
	FILE *fp, *fp1;
	char *linebuf = NULL;
	size_t linebuf_size = 0;

	if(argc < 2)
	{
		fprintf(stderr,"Usage....\n");
		exit(1);
	}

	fp = fopen(argv[1],"r");
	fp1 = fopen(argv[1],"r");
	if(fp == NULL)
	{
		perror("fopen()");
		exit(1);
	}

	while(1)
	{
		if( (ret = mygetline(&linebuf,&linebuf_size,fp)) < 0)
			break;
		printf("linebuf_size = %d\n",linebuf_size);
		printf("strlen  = %d\n",strlen(linebuf));
		printf("ret = %d\n", ret);
		printf("%s", linebuf);
	}
	printf("--------------------------------------\n");
	printf("--------------------------------------\n");
	printf("--------------------------------------\n");

	while(1)
	{
		if( (ret = getline(&linebuf,&linebuf_size,fp1)) < 0)
			break;
		printf("linebuf_size = %d\n",linebuf_size);
		printf("strlen = %d\n",strlen(linebuf));
		printf("ret = %d\n", ret);
		printf("%s", linebuf);
	}


	fclose(fp);

	exit(0);
}

