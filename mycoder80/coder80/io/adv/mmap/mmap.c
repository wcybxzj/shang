#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define FNAME	"/etc/services"

int main()
{
	char *str;		
	struct stat statres;
	int fd,count,i;

	fd = open(FNAME,O_RDONLY);
	if(fd < 0)
	{
		perror("open()");
		exit(1);
	}

	if(fstat(fd,&statres) < 0)
	{
		perror("fstat()");
		exit(1);
	}

	str = mmap(NULL,statres.st_size,PROT_READ,MAP_SHARED,fd,0);
	if(str == MAP_FAILED)
	{
		perror("mmap()");
		exit(1);
	}

	close(fd);

	for(i = 0 ; i < statres.st_size; i++)
		if(str[i] == 'a')
			count++;	

	printf("%d\n",count);

	munmap(str,statres.st_size);

//	close(fd);

	exit(0);
}



