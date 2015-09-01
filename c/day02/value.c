#include <stdio.h>
#include <math.h>

int main(void)
{
	int a, b, c;
	int delta;
	int x1, x2;
	
	printf("请输出一元二次方程的系数(ax^2+bx+c):");	
	scanf("%d %d %d", &a, &b, &c);

	if (a == 0) {
		printf("此方程不是一个一元二次方程\n");
	} else {
		delta = b*b - 4*a*c;
		if (delta < 0) {
			printf("此方程无解\n");
		} else if (delta == 0) {
			x1 = x2 = (-b) / (2*a);	
			printf("此方程有两个相同的解为:x1 == x2 == %d\n", x1);
		} else {
			x1 = -b + sqrt(delta) / (2*a);
			x2 = -b - sqrt(delta) / (2*a);
			printf("%dx*x + %dx + %d = 0的解分别为:%d, %d\n", a, b, c, x1, x2);
		}
	}

	return 0;
}
