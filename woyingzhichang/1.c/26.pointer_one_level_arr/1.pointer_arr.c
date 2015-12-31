#include <stdio.h> 
//a[i] = *(a+i) =  *(p+i) = p[i]
//&a[i] = a+i = p+i = &p[i]
//相同点:所有用到数组名a的地方,都可以用p替代

//不同点:a是一个数组名是一个表示地址的常量,p是一个变量. 
//a++ 报错:自增操作必须是左值, p++ ok

int main(int argc, const char *argv[])
{
	int a[3] = {1, 2, 3};
	int i;
	int *p = a;

	//for (i = 0; i < sizeof(a)/sizeof(a[0]); i++) {
	for (i = 0; i < sizeof(a)/sizeof(*a); i++) {
		printf("%p -> %d\n", a+i, a[i]);
		printf("%p -> %d\n", a+i, *(a+i));
		printf("%p -> %d\n", p+i, p[i]);
		printf("%p -> %d\n", p+i, *(p+i));
	}
	return 0;
}
