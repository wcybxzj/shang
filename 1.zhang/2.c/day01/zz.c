#include <stdio.h>

int main(void)
{	
	/*
	 	定义和声明区别：
			定义占用存储空间
		外部声明：extern
		初始化：定义的时候同时赋值
		定义一个变量的修饰：
		自动变量：
			auto int a;
			int a;
		静态变量：只初始化一次
			static int a;
		寄存器变量：
			register int a;
		只读
			const int a;
		易失变量:防止编译优化	
			volatile int a;
		全局变量：
			在函数体外定义的变量
			如果未初始化，值为0
		局部变量：
			在函数内部定义的变量
			如果未初始化，随机值
		区别：
			作用域
				全：整个文件
				局：函数内
			生存周期
				全：整个进程结束
				局部：函数结束
	 */

	int var0 = 1, var2 = 3;
	int var3, var4;

	var3 = (var0++)+var2;
	var4 = ++var0+var2++;
	//++在后，先参与运算再自增
	//++在前，先自增再参与运算

	printf("var0 = %d, var2 = %d, var3 = %d, var4 = %d\n", \
			var0, var2, var3, var4);

	return 0;
}
