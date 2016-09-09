#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define SIZE 1024

int main(int argc, const char *argv[])
{
	int fd, len;
	char buf[SIZE];
	if(argc == 1){
		printf("1\n");
	}else{
		len = read(0, buf, SIZE);
		//write(1, buf ,len);
		fd = open("/dev/stdout", O_RDWR);
		write(fd, buf ,len);
		printf("%s\n", buf);
		
	}
	return 0;
}

