#include <stdio.h>
#include <stdlib.h>
int m;
char c;
int n;

//最后需要说明的是：内存对齐不是C语言的特性，它属于计算机的运行原理，C++、Java、Python等其他编程语言同样也会有内存对齐的问题。

//对于全局变量，GCC在 Debug 和 Release 模式下都会进行内存对齐，
//而VS只有在 Release 模式下才会进行对齐。
//
//而对于局部变量，GCC和VS都不会进行对齐，不管是Debug模式还是Release模式。
int main(){
    printf("&m: %X\n&c: %X\n&n: %X\n", &m, &c, &n);

	int x;
	char y;
	int z;
    printf("&x: %X\n&y: %X\n&z: %X\n", &x, &y, &z);
    system("pause");
    return 0;
}
