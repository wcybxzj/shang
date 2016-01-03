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

	//fseek(fp, 1LL*1024LL*1024LL*1024LL ,SEEK_SET);
	fseek(fp, 10, SEEK_SET);
	fputc('y', fp);
	return 0;
}
