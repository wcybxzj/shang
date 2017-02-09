#include <stdio.h>
#include <stdlib.h>
int main(int argc, const char *argv[])
{
	int *p=NULL;
	//段错误:
	//因为局部变量还是随机值,下面操作一个值为随机值的指针
	//int *p;

	*p =123;
	return 0;
}
