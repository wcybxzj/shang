#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

//使用不同方法,目的让hello写到文件中去
int main(int argc, const char *argv[])
{
	int fd = open("/tmp/1.txt", O_CREAT|O_TRUNC|O_RDWR);
	assert(fd>0);

	int fd2 = open("/tmp/2.txt", O_CREAT|O_TRUNC|O_RDWR);
	assert(fd2>0);

	int fd3 = open("/tmp/3.txt", O_CREAT|O_TRUNC|O_RDWR);
	assert(fd3>0);

	//测试1:内容进入fd
	//dup2(fd, STDOUT_FILENO);
	//puts("1123344");

	//测试2:内容都进入fd3
	//char *str = "123";
	//dup2(fd2, fd);//多余了
	//dup2(fd3, fd);
	//write(fd, str, strlen(str));

	//测试3:对内容都进入fd
	char *str2 = "22";
	char *str3 = "33";
	dup2(fd, fd2);
	dup2(fd, fd3);
	write(fd2, str2, strlen(str2));
	write(fd3, str3, strlen(str3));

	return 0;
}
