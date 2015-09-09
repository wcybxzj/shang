#include <stdio.h>
int main(int argc, const char *argv[])
{
	int w[2][3] = {
		{11,22,33},
		{44,55,66}
	};

	int (*pw)[3];
	pw = w;

	printf("================= 30题 =====================\n");

	//A
	printf("A:\n");
	printf("%d\n",  *(w[0]+2));//33

	//B w只有2行所以1+2已经超过范围所以选这个
	printf("B:\n");
	printf("%d\n", *(pw+1)[2]);//
	printf("%d\n", *(*(pw+1+2)));//解释B

	//C
	printf("C:\n");
	printf("%d\n", pw[1][1]);//55

	//D
	//printf("D:\n");
	printf("%d\n", *(pw[1]+2));//66


	printf("================= 28题 =====================\n");



	return 0;
}
