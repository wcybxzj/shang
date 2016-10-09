#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FNAME "/etc/services"
#define FNAME2 "/tmp/num.txt"
#define BUF_SIZE 10
//单进程存储映射IO实例
int main(void){
	int len;
	char str_num[BUF_SIZE];
	char *str, *str2;
	int i, fd, fd2, count=0;
	struct stat st;
	fd =open(FNAME, O_RDONLY);
	if(fd<0){
		perror("open()");
		exit(1);
	}
	fd2 =open(FNAME2, O_RDWR|O_CREAT|O_TRUNC);
	if(fd2<0){
		perror("open()");
		exit(1);
	}
	if (stat(FNAME, &st)<0) {
		perror("stat():");
		exit(1);
	}

	//
	str = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if(str == MAP_FAILED){
		perror("mmap():");
		exit(1);
	}
	close(fd);
	for(i=0; i<st.st_size;i++){
		if(str[i] == 'a'){
			count++;
		}
	}
	munmap(str, st.st_size);

	//
	sprintf(str_num, "%d", count);
	len = strlen(str_num);
	ftruncate(fd2, len);//知识点
	str2 = mmap(0, len, PROT_WRITE|PROT_READ, MAP_SHARED, fd2, 0);
	if(str2 == MAP_FAILED){
		perror("mmap():");
		exit(1);
	}
	close(fd2);
	strcpy(str2, str_num);
	munmap(str2, len);
	exit(0);
}
