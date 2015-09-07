#include <stdio.h>
#include <string.h>

#define SIZE 11

char *itoa(char *p, int num);

int _itoa_r(char *p, int data)
{
	int index;
	char ch;

	if (data == 0) {
		return 0;
	}	
	
	ch = data%10 + '0';
	index = _itoa_r(p, data/10);
	p[index] = ch;

	return index + 1;
}

char *itoa_r(char *p, int num)
{
	if (num == 0) {
		p[0] = '0';
		p[1] = '\0';
	} else {
		int ind = _itoa_r(p, num);
		p[ind] = '\0';	
	}	

	return p;
}

int main(void)
{
	char str[SIZE] = {};

	itoa_r(str, 12345);
	printf("%s\n", str);
	printf("%s\n", itoa_r(str, 1234567890));

	return 0;
}

void reverse(char *p, int len)
{
	char *q = p + len - 1;	

	while (p < q) {
		*p  = (*p) ^ (*q);
		*q  = (*p) ^ (*q);
		*p  = (*p) ^ (*q);
		p++, q--;
	}
}

char *itoa(char *p, int num)
{
	//12345
	char *ret = p;
	char *q = p;

	while (num) {
		*p = num % 10 + '0';
		num /= 10;
		p++;
	}
	*p = '\0';

	reverse(ret, strlen(ret));

	return ret;
}
