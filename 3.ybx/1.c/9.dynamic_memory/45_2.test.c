#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


//通过子函数malloc空间给main去使用
//方法1:
void func(int **p, int num)
{
	*p = malloc(sizeof(int)*num);
	if (NULL == *p) {
		printf("%s\n", strerror(errno));
		exit(1);
	}
	return;
}

//方法2:
void* func1(int num){
	int *p = NULL;
	p = malloc(sizeof(int));
	return p;
}

int main(int argc, const char *argv[])
{
	int *p = NULL;;
	int num = 1;
	func(&p, num);
	*p = 123;
	printf("%p-->%d\n", p, *p);
	free(p);

	int *p1 = NULL;
	p1 = func1(num);
	*p1 =456;
	printf("%p-->%d\n", p1, *p1);
	free(p1);


	return 0;
}
