#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define SIZE 1

//fd 指向需要的行的头
//小于0失败
int get_line(int fd, int want_row){
	lseek(fd, 0, SEEK_SET);
	int ret, row_num = 0;
	char buf[SIZE];
	while (1) {
		ret = read(fd, buf, SIZE);
		if (ret == 0) {//EOF
			return;
		}
		if (ret < 0) {//error
		perror("read():");
			exit(1);
		}
		row_num++;
		if (row_num == want_row) {
			//
		}
	}
}

//删除第10行
//fd1 定位到11行
//fd2 定位到10行
int main(int argc, const char *argv[])
{
	//保证文件必须存在，r/r+　都不会取创建不存在的文件
	int fd1 = open("1.txt", O_RDONLY);//r 
	int fd2 = open("1.txt", O_RDWR);//r+
	if (fd1 < 0 || fd2 < 0) {
		perror("open():");
		exit(1);
	}

	int big_line=11, small_line=10, ret;
	while (1) {
		ret = get_line(fd1, big_line);
		if (!ret) {
			break;
		}
		ret = get_line(fd2, small_line);
		if (!ret) {
			break;
		}

		//clean(fd2, small_line);
		//while (read(fd1)) {
		//	write(fd2);
		//}

		big_line++;
		small_line++;
	}
	//tuncate();

	return 0;
}


int  get_row_(int fd, int num)
{

}

