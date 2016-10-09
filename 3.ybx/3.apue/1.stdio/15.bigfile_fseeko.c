#define _FILE_OFFSET_BITS 64
#include <stdio.h>
int main(int argc, const char *argv[])
{

	//创建空洞文件
	//od -t c bigfile.txt
	FILE *fp;
	fp = fopen("bigfile.txt", "w");
	if (NULL == fp) {
		perror("fopen()");
		return 1;
	}
	//避免数据溢出 要在每个数字加单位
	//最后减最后要一个系统调用写一个字符
	//off_t size = 5LL*1024LL*1024LL*1024LL*1024LL-1LL;
	off_t size = 3LL*1024LL*1024LL*1024LL-1LL;
	printf("%lld\n",size);
	fseeko(fp, size, SEEK_SET);
	fputc('\0',fp);//需要刷新内容到文件
	sleep(5);
	return 0;
}
