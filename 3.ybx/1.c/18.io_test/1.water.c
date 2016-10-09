#include <stdio.h>
#include <math.h>
#include <stdlib.h>


#define WEIGHT_OF_A_WATER 950
#define A_MOLECULE_GRAM 3.0e-23

void water()
{
	//题1:
	//1个水分子3.0e-23g
	//输入夸脱求,水分子个数 ,一夸脱水950克
	int quart = 0;
	double all_gram;
	double num;
	printf("输入水的夸脱数\n");
	scanf("%d", &quart);
	if (quart<=0) {
		fprintf(stderr, "Input error\n");
		exit(1);
	}
	all_gram = quart * WEIGHT_OF_A_WATER;//总克数
	num = all_gram/A_MOLECULE_GRAM;
	printf("%e\n", num);
}

void angle()
{
	//题2:
	printf("---------------------------------\n");
	printf("输入三角形三边长度,求面积\n");
	float a, b, c, s, area;
	int ret;
	ret = scanf("%f%f%f",&a,&b,&c);
	if (ret!=3) {
		printf("输入失败\n");
	}else{
		printf("输入成功\n");
	}
	if(!(a+b>c) || !(a+c>b) ||!(b+c>a)) {
		fprintf(stderr, "a b c error");
		exit(1);
	} 
	//s = 1/2 * (a + b + c);// bug 因为1/2,得0
	s = 0.5 * (a + b + c);
	area = sqrt(s * (s-a) * (s-b) *(s-c));
	printf("area %f\n", area);
}

void root()
{
	int a,b,c;
	int x1, x2, ret;
	printf("plz enter 3 number to get root\n");
	scanf("%d%d%d",&a,&b,&c);
	if ( (ret = b*b - 4*a*c) <= 0) {
		fprintf(stderr, "root have not a answer!!!\n");
		exit(1);
	}

	x1 = (-b+sqrt(ret))/2*a;
	x2 = (-b-sqrt(ret))/2*a;
	printf("x1:%d\n",x1);
	printf("x2:%d\n",x2);

}

int main(int argc, const char *argv[])
{
	root();
	return 0;
}
