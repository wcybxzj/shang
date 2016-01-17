#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{
	puts(getenv("PATH"));
	//getchar();//pmap 进程id 观察内存布局
	return 0;
}
