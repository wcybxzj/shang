#include <stdio.h>
#if 0
	1.printf的参数是从右向左遍历
	2.printf的返回值是成功输出字符的个数
	常见的格式占位符
		%c：输出字符
		%d: 输出整形
		%ld: 长整形
		%s:	输出字符串
		%f: 单精度浮点
		%lf: 双精度浮点
		%p:	地址
		%x: 十六进制
		%X: 字母大写
		%o: 八进制
	转义字符：
		\n:换行
		\r:回车
		\t:制表符
		\b:退格	
	
#endif

int main(void)
{
	int a = 100;
	int b = 2;
	int ret;

	ret = printf("\tthe \bnumber of a is %d\n", a++, b = ++a);
	printf("a = %d, \rb = %d, ret = %d\n", a, b, ret);

	return 0;
}

