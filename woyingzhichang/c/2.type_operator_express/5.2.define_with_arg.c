#include <stdio.h>
#define MAX(a, b)((a) > (b)? (a) : (b))

#define MAX_BEST(a, b)\
	({typeof(a) A = a, B = b; ( (A) > (B) ? (A) : (B) );})

int max(int a, int b){
	return a > b ? a : b;
}

//想要的结果:
//5,3
//5
//6,4
int main(int argc, const char *argv[])
{
	int i = 5, j = 3;

	//失败,比较大的值会自增２次
	//i = 5, j =3
	//6
	//i = 7, j =4
	printf("i = %d, j =%d\n",i , j);
	printf("%d\n", MAX(i++, j++));
	printf("i = %d, j =%d\n",i , j);

	printf("----------------------\n");

	//方法1:函数
	i=5, j=3;
	printf("i = %d, j =%d\n",i , j);
	printf("%d\n", max(i++, j++));
	printf("i = %d, j =%d\n",i , j);
	
	printf("----------------------\n");
	//方法2:GNU宏,非标准,vim会报错发红
	i=5, j=3;
	printf("i = %d, j =%d\n",i , j);
	printf("%d\n", MAX_BEST(i++, j++));
	printf("i = %d, j =%d\n",i , j);

	return 0;
}



