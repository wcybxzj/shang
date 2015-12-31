#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

off_t flen(const char *fname)
{
	struct stat statres;
	
	if(stat(fname,&statres) < 0)
	{
		perror("stat()");
		exit(1);
	}

	return statres.st_size;

}

int main(int argc,char **argv)
{
	if(argc < 2)
	{
		fprintf(stderr,"Usage...\n");
		exit(1);
	}

	printf("%lld\n",(long long)flen(argv[1]));
	
	exit(0);
}


