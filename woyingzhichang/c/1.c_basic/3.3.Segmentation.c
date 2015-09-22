#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, const char *argv[])
{
	FILE *fp;
	fp = fopen("tmp.txt","r");//文件不存在返回空指针设置errno
	if (NULL == fp) {
		//strerror把errno 变成对应的错误信息
		fprintf(stderr, "fopen: %s", strerror(errno));
	}

	return 0;
}
