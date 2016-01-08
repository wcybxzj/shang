#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <string.h>
int main(int argc, const char *argv[])
{
	int ret;
	FILE *fp;
	fp = fopen("1.xtt", "w");
	off_t size = 5LL*1024LL*1024LL*1024LL ;
	char arr[] = "this is a file!";
	while (1) {
		//ret = fwrite(arr, strlen(arr),size, fp);
		fputc('a',fp);
		printf("ret is %d\n", ret);
	}
	return 0;
}
