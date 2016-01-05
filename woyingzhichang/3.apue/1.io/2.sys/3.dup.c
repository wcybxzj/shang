#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FNAME "/tmp/out"

//让hello写到文件中去
int main(int argc, const char *argv[])
{
	////场景1:
	//int fd;
	//close(1);
	//fd = open(FNAME, O_WRONLY|O_CREAT|O_TRUNC, 0600);
	///**************************************/
	//puts("hello");

	//场景2:
	close(1);
	int fd;
	fd = open(FNAME, O_WRONLY|O_CREAT|O_TRUNC, 0600);
	close(1);
	dup(fd);
	//close(fd);
	/**************************************/
	puts("hello7");


	return 0;
}
