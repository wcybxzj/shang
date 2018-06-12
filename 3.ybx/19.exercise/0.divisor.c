#include <stdio.h>
//对比num/2+1和num+1/2的区别
int main(int argc, const char *argv[])
{
	int i;
	for (i = 0; i < 10; i++) {
		printf("i:%d\n",i);
		printf("(i/2)+1=%d\n",i/2+1);
		printf("(i+1)/2=%d\n",(i+1)/2);
	}
	return 0;
}
