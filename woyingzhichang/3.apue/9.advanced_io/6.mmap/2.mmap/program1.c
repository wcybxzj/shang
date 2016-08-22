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
#define WORD "hello"

int main(void){
	char *str;
	int fd, len;
	fd = open(FNAME, O_RDWR|O_RDONLY|O_CREAT);
	if(fd < 0){
		perror("open():");
		exit(0);
	}
	ftruncate(fd, SIZE);

	str = mmap(0, SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(str == MAP_FAILED){
		perror("mmap():");
		exit(1);
	}
	close(fd);
	strcpy(str, WORD);
	printf ("%s\n", str);
	munmap(str, SIZE);
	exit(0);
}
