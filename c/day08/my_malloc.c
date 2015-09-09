#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//整形
void func(int **p)
{
	*p = malloc(sizeof(int));
	**p = 123;
}

//一维数组
void one_level_arr(int (**p)[3], int num)
{
	srand(getpid())
	*p = malloc(sizeof(int) * num);
	for (i = 0; i < num; i++) {
		*(*p+i) = rand() % 100;
	}
}

int main(int argc, const char *argv[])
{
	printf("========== 普通指针 =============\n");
	int *p = NULL;
	func(&p);
	printf("%d\n", *p);

	printf("========== 一维数组 ==============\n");
	int (*p1)[3]=NULL;
	one_level_arr(&p1, 3);


	return 0;
}

