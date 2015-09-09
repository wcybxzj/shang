#include <stdio.h>

int main(int argc, char *argv[])
{
	int num1, num2;
	int res;
	if (argc < 4) {
		fprintf(stderr, "Usage:%s ...\n", argv[0]);
		return 1;
	}

	num1 = atoi(argv[1]);
	num2 = atoi(argv[3]);

	switch (*argv[2]) {
		case '+':
			res = num1 + num2; break;
		case '-':
			res = num1 - num2; break;
		case 'x':
			res = num1 * num2; break;
		case '/':
			res = num1 / num2; break;
		default:
			fprintf(stderr, "input error\n");
			return 1;
	}

	printf("%d %c %d = %d\n", num1, *argv[2], num2, res);

	return 0;
}
