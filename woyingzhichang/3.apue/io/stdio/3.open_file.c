#include <stdio.h>
//测试能打开多少文件资源
//ulimit -a open_files
int main(int argc, const char *argv[])
{
	int i =0;
	FILE *fp = NULL;
	while (1) {
		fp = fopen("tmp", "w");
		if (NULL == fp) {
			break;
		}
		i++;
	}
	printf("%d\n", i);
	return 0;
}
