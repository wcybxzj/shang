#include <stdio.h>
#include <string.h>
#include <unistd.h>

//文件是全缓冲
//命令行是行缓冲
int main(int argc, const char *argv[])
{
	FILE *fp1 = fopen("1.txt", "r");
	FILE *fp2 = fopen("2.txt", "w+");//每次都没清空

	char str1[100];
	char *str = NULL;
	while ( fgets(str1, 100, fp1) ) {
		fputs(str1, fp2);
		printf("%s",str1);
		sleep(1);
	}
	return 0;
}
