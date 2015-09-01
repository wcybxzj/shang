#include <stdio.h>

int main(void)
{
	int a, b;
	char op;
	int res;
	
	printf("输入表达式\n");
	scanf("%d %c %d", &a, &op, &b);

	switch (op) {
		case '+':
			res = a + b; break;
		case '-':
			res = a - b; break;
		case '*':
			res = a * b; break;
		case '/':
			res = a / b; break;
		case '%':
			res = a % b; break;
		default :
			return -1;
	}
	printf("%d %c %d = %d\n", a, op, b, res);

	return 0;
}
