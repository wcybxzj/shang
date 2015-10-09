#include <stdio.h>
#include <stdlib.h>

int main(int argc,char *argv[])
{
	FILE *fps,*fpd;
	int ch;

	if(argc < 3)
	{
		fprintf(stderr,"Usage:%s <srcfile> <destfile>\n",argv[0]);
		exit(1);
	}

	fps = fopen(argv[1],"r");
	if(fps == NULL)
	{
		perror("fopen()");
		exit(1);
	}

	fpd = fopen(argv[2],"w");
	if(fpd == NULL)
	{
		perror("fopen()");
        exit(1);
	}

	while((ch = fgetc(fps)) != EOF)
		fputc(ch,fpd);

	fclose(fpd);
	fclose(fps);	

	exit(0);
}


