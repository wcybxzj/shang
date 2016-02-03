#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define SIZE 1024

//先运行这句 cat /etc/services > 1.txt
//a+ 读时候　文件位置指针在文件首
int main(int argc, const char *argv[])
{
	char str[SIZE];
	FILE *fp;
	fp = fopen("1.txt", "a+");

	if (NULL == fp) {
		perror("fopen():");
		exit(1);
	}

	while (fgets(str, SIZE, fp)) {
		printf("%s", str);
		break;
	}

	snprintf(str, SIZE, "%s\n", "ybx");
	fputs(str, fp);

	return 0;
}
