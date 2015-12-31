#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

//这个程序需要到linux 看效果
//strerror缺少头文件,程序会认为strerror返回int，所以段错误
int main(int argc, const char *argv[])
{
	FILE *fp;
	fp = fopen("tmp.txt","r");//文件不存在返回空指针设置errno
	if (NULL == fp) {
		//strerror把errno 变成对应的错误信息
		//不包含头文件相当于 printf("%s\n",1);
		fprintf(stderr, "fopen: %s", strerror(errno));
	}

	return 0;
}
