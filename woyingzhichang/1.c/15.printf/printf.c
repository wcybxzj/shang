#include <stdio.h>
int main(int argc, const char *argv[])
{
	float abc =123.456;
	printf("%010.2f\n",abc);//一共10位小数后有2位,前边用0补齐

	char str[10] ="abcdefhig";
	printf("%.3s\n", str);//只输出3位

	printf("%10.3s[stop]\n", str);//全长10 右对齐 只输出3位

	int int_num = 255;
	printf("%#x\n", int_num);//#输出8进制前补0,16进制前补0x
	printf("%#o\n", int_num);

	float float_abc =123.456;
	printf("%lf\n",float_abc);//lf 就是double


	long long l = 123456;
	printf("l = %lld\n", l);//long long int

	return 0;
}
