#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ssize_t mygetline(char **lineptr, size_t *n, FILE *stream)
{


}

int main(int argc,char **argv)
{
	FILE *fp;
	char *linebuf = NULL;
	size_t linebuf_size = 0;

	if(argc < 2)
	{
		fprintf(stderr,"Usage....\n");
		exit(1);
	}

	fp = fopen(argv[1],"r");
	if(fp = NULL)
	{
		perror("fopen()");
		exit(1);
	}

	while(1)
	{
		if(mygetline(&linebuf,&linebuf_size,fp) < 0)
			break;
		printf("linebuf_size = %d\n",linebuf_size);
		printf("%d\n",strlen(linebuf));
	}

	fclose(fp);

	exit(0);
}

