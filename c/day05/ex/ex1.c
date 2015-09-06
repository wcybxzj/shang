#include <stdio.h>

//获取数据占用空间
int my_sizeof(){
	int cnt = 0;
	unsigned int tmp = ~0u;//32个1
	while (tmp >> cnt++ != 0x1);//右移判断有多少个1
	return cnt;
}

//10进制用2进制打印
//使用位操作进行
int binary_print(int num)
{
	int i;
	for (i = my_sizeof()-1; i>=0;i--) {
		(num >> i) & 0x1 ? putchar('1'):putchar('0');
		if (i%8 == 0) {
			printf(" ");
		}
	}
	printf("\n");
	return 0;
}

int main(int argc, const char *argv[])
{
	unsigned int var;
	binary_print(125);
	binary_print(12);

	return 0;
}
