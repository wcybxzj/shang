#include <stdio.h>

//把二维数组当一维数组使用,要取某行某列很麻烦,引出数组指针
int main(int argc, const char *argv[])
{
	int a[2][3] = {1, 2, 3, 4, 5, 6};
	int i,j;

	int *p;
	//p = a;//error  类型不匹配, p相等于列指针，a相当于行指针 
	//p = *a;//ok
	p = &a[0][0];//ok

	for (i = 0; i < 6; i++) {
		//printf("%d\n", p[i]);
		printf("%d\n", *(p+i));
	}

	return 0;
}
