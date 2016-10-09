#include <stdio.h>

//int 范围完全覆盖char
//char是一个字节8位
//有符号char -128到127
//无符号char 0到255

//int 完全可以替代char
//参数,返回值都是int, 调用中直接用int
int func1(int a){
	return 'a';
}

//不使用char的原因就是面临数据溢出的问题
//在标准c没有规定char是否是有符号类型,各个实现是不同的
//gcc waring:类型溢出
//这就是为什么int getc(); 因为需要返回负数出错时候
char func2(){
	return -10000;
}

//此程序表达为什么尽量用int替代char
int main(int argc, const char *argv[])
{
	char val;
	val = func1('b');
	printf("%c\n", val);

	char tmp;
	tmp = func2();
	printf("%d\n", tmp);

	return 0;
}
