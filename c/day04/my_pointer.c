#include <stdio.h>
int main(int argc, const char *argv[])
{
	int arr[] = {1, 2, 3};
	int *p;
	//情况1:
	//printf("=======1==========\n");
	//*p =arr;//段错误,p是一个为初始化值,这里要对未初始化的的内存去保存值

	//情况2:
	printf("=======2==========\n");
	p = arr;
	printf("%d\n",*p);//1
	printf("%d\n",*(p+1));//2
	printf("%d\n",*(p+2));//3

	//情况3:
	printf("=======3==========\n");
	char *r, q;
	printf("%d\n", sizeof(r));//8
	printf("%d\n", sizeof(q));//1

	//情况4:char*类型保存 保存一个int的数据,能报警但是输出的值不正确
	printf("=======4==========\n");
	int a = 100000;//10w
	char *char_p;
	char_p = &a;
	printf("%d\n", *char_p);

	//情况5：指针类型的意思,因为指针类型为int,所以才能在这出值
	printf("=======5==========\n");
	printf("%d\n", *arr);


	printf("=======6=======\n");
	int *b;
	b = &a;
	printf("%d\n", *b);
	return 0;
}
