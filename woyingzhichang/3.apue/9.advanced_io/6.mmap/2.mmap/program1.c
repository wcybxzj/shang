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
	char *word ="go";
	int len = strlen(word);
	char *str;
	int fd;
	fd = open(FNAME, O_RDWR|O_RDONLY|O_CREAT);
	if(fd < 0){
		perror("open():");
		exit(0);
	}
	ftruncate(fd, len);

	str = mmap(0, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(str == MAP_FAILED){
		perror("mmap():");
		exit(1);
	}
	close(fd);
	strcpy(str, word);
	printf ("%s\n", str);
	munmap(str, len);
	exit(0);
}
