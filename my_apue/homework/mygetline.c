#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define START_SIZE 10
#define INC_SIZE 2

int get_fushu(int a){
	return (a^0xffffffff)+1;
}
//有2个问题：
//malloc 重复分配  
//一行数据一定有一个\n,文件尾一定有EOF
ssize_t mygetline(char **lineptr, size_t *n, FILE *stream)
{
	int finish = 0;
	int num = 0, c, i = 0, back_num = 0;
	int buffsize = START_SIZE;
	int pos;

	*lineptr = malloc(buffsize);
	if (NULL == *lineptr) {
		return -1;
	}

	while (1) {
		c = getc(stream);
		if (c=='\n') {
			break;
		}
		if (c==EOF) {
			return -2;
		}
		num++;
	}



	if ((num > buffsize)) {
		while(num > buffsize) {
			buffsize+=INC_SIZE;
		}
		printf("buffsize:%d\n", buffsize);
		*lineptr = realloc(*lineptr, buffsize);
		if (NULL== *lineptr) {
			return -1;
		}
	}

	back_num = get_fushu(num);
	//printf("%d\n", back_num);
	pos = fseek(stream, back_num, SEEK_CUR);
	if (pos !=0) {
		//perror("pos");
		return -1;
	}

	while ((c= getc(stream)) != EOF && c != '\n') {
		//printf("%c\n", c);
		(*lineptr)[i] = c;
		i++;
	}

	*n = buffsize;

	if (finish) {
		(*lineptr)[i] = EOF;
		return -1;
	}else{
		(*lineptr)[i] = '\n';
		return num;
	}
}

int main(int argc,char **argv)
{
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

	//mygetline(&linebuf,&linebuf_size,fp);
	//printf("linebuf_size = %d\n",linebuf_size);
	//printf("%d\n",strlen(linebuf));

	while(1)
	{
		if(mygetline(&linebuf,&linebuf_size,fp) < 0)
			break;
		printf("linebuf_size = %d\n",linebuf_size);
		printf("%d\n",strlen(linebuf));
		printf("%s", linebuf);
	}

	printf("--------------------------------------\n");

	while(1)
	{
		if(getline(&linebuf,&linebuf_size,fp1) < 0)
			break;
		printf("linebuf_size = %d\n",linebuf_size);
		printf("%d\n",strlen(linebuf));
		printf("%s", linebuf);
	}


	fclose(fp);

	exit(0);
}

