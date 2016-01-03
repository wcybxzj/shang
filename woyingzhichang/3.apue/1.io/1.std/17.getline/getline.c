#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(int argc, const char *argv[])
{
	FILE *fp;
	char *str;
	int num;
	int read;

	fp = fopen("services", "r");
	if (NULL == fp) {
		perror("fopen():");
		exit(1);
	}
	//问题1:
	//必须
	//getline的机制是malloc+realloc,
	//所以需要NULL判断是否是第一分配地址,不初始化就是野指针,段错误
	str = NULL;
	while ((read = getline(&str, &num, fp))>=0) {
		//printf("content:%s",str);
		printf("strlen(str)%d\n", strlen(str));
		printf("read:%d\n", read);
		printf("num:%d\n",num);
	}

	//问题2:
	//getline存在内存泄露的问题,没有回收空间的函数，
	//但这种内存泄露是可控的内存泄露，泄露的内存指向str,大小为num
	//也可以free(str); 但不推荐,因为可能是c++ new分配的空间就不能free()
	free(str);

	//问题3:
	//getline也是方言所以需要自己去实现 大作业1

	exit(0);
}
