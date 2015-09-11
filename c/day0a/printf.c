#include <stdio.h>

int main(void)
{
	char buf[] = "dinghonghong 12|3.14";	
	char str[32] = {};
	int num;
	float fl;

	sscanf(buf, "%s%d|%f", str, &num, &fl);

	fprintf(stdout, "%s %d %f\n", str, num, fl);

	return 0;
}
