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
	off_t size = 5LL*1024LL*1024LL*1024LL*1024LL ;
	//off_t size = 5*1024*1024*1024;
	printf("%lld\n",size);
	fseeko(fp, size, SEEK_SET);
	fputc('y', fp);
	return 0;
}
