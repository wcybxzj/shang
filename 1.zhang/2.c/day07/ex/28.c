#include <stdio.h>
int main(int argc, const char *argv[])
{
	int (*cp)[5];
	int c[4][5] = {
		{11,22,33,44,55},
		{66,77,88,99,100},
		{101,102,103,104,105},
		{41,49,43,44,45}
	};

	cp = c;

	printf("cp is %p\n", cp);
	printf("A:=====================\n");
	printf("%p\n", cp+1);//cp后5x4字节 c[1]的地址

	printf("B:=====================\n");
	printf("%p\n", cp+3); //&c[3] 3x5x4字a节 等于 &c[3]
	printf("%p\n", *(cp+3)); //c[3] 3x5x4字节 等于 &c[3][0]

	printf("%d\n", **(cp+3)+1);//41 + 1 =42
	printf("%d\n", *(*(cp+3)+1));

	printf("C:=====================\n");
	printf("%p\n", *(cp+1)+3); //&c[1][3]

	printf("D:=====================\n");
	printf("%d\n", *(*cp + 2));//c[0][2]
	return 0;
}
