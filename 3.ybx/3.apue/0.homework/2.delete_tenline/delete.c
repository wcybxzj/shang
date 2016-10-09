#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SIZE 1

int get_fliesize(char *name);
int get_rownum(char *name);
int get_rowsize(char *name, int want_row);
int get_line(int fd, int want_row);


//删除第10行
//fd1 定位到11行
//fd2 定位到10行
int main(int argc, const char *argv[])
{
	//保证文件必须存在，r/r+　都不会取创建不存在的文件
	FILE *fp;
	int fd1 = open("1.txt", O_RDONLY);//r 
	int fd2 = open("1.txt", O_RDWR);//r+
	if (fd1 < 0 || fd2 < 0) {
		perror("open():");
		exit(1);
	}
	int need_truncate = 0;
	char buf[SIZE];
	int big_line=3, small_line=2, ret;
	int total_size, lastrow_size, row_num;
	while (1) {
		ret = get_line(fd1, big_line);
		//printf("%d\n", ret);
		if (ret<0) {
			printf("can not get big_line:%d\n", big_line);
			goto after;
			printf("never do it \n");
		}
		ret = get_line(fd2, small_line);
		//printf("%d\n", ret);
		if (ret<0) {
			printf("can not get small_line:%d\n", small_line);
			goto after;
			printf("never do it \n");
		}
		need_truncate = 1;
		while (1) {
			ret = read(fd1, buf, SIZE);
			if (ret == 0) {//EOF
				goto after;
				break;
			}
			if (ret < 0) {
				perror("read():");
				exit(2);
			}
			write(fd2, buf, SIZE);
			if (*buf == '\n') {
				break;
			}
		}
		big_line++;
		small_line++;
	}

after:
	if (need_truncate) {
		printf("ttt\n");
		total_size = get_fliesize("1.txt");
		row_num = get_rownum("1.txt");
		lastrow_size = get_rowsize("1.txt", row_num);
		ftruncate(fd2, total_size-lastrow_size);//fd 一定要写权限
		//truncate("1.txt", total_size-lastrow_size);
	}

	return 0;
}

//文件总大小
int get_fliesize(char *name){
	int fd, size=0;
	char buf[SIZE];
	fd = open(name, O_RDONLY);
	while (read(fd, buf, SIZE)) {
		size++;
	}
	close(fd);
	return size;
}

//文件总行数
int get_rownum(char *name){
	int fd, rownum=0;
	char buf[SIZE];
	fd = open(name, O_RDONLY);
	while (read(fd, buf, SIZE)) {
		if (*buf == '\n') {
			rownum++;
		}
	}
	close(fd);
	return rownum;
}

//获取特定行的大小
int get_rowsize(char *name, int want_row){
	int fd, ret, current_row=0, size=0;
	char buf[SIZE];
	fd = open(name, O_RDONLY);
	while (1) {
		ret = read(fd, buf, SIZE);
		if (ret == 0) {//NULL
			return 0;
		}
		if (ret < 0) {
			return 0;
		}
		size++;
		if (*buf == '\n') {
			current_row++;
			if (want_row == current_row) {
				return size;
			}
			size = 0;
		}
	}
	close(fd);
	return size;
}

//fd 指向需要的行的头
//小于0失败
int get_line(int fd, int want_row){
	lseek(fd, 0, SEEK_SET);
	char buf[SIZE];
	int ret, row_num = 0, word_num = 0;
	while (1) {
		ret = read(fd, buf, SIZE);
		if (ret == 0) {//EOF
			return -1;
		}
		if (ret < 0) {//error
			perror("read():");
			exit(1);
		}
		word_num++;
		if (*buf == '\n') {
			row_num++;
			if (row_num == want_row) {
				//lseek to row begining
				lseek(fd, -word_num, SEEK_CUR);
				return 1;
			}
			word_num = 0;
		}
	}
	return -2;
}
