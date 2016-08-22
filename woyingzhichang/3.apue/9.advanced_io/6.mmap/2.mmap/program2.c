#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FNAME "./mmap.data"

#define SIZE 1024

int main(void){
	char *str;
	int fd;
	fd = open(FNAME,O_RDONLY);
	if(fd < 0){
		perror("open():");
		exit(0);
	}

	str = mmap(0, SIZE, PROT_READ, MAP_SHARED, fd, 0);
	if(str == MAP_FAILED){
		perror("mmap():");
		exit(1);
	}
	printf ("%s\n", str);
	munmap(str, SIZE);
	exit(0);
}
