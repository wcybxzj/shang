#include <stdio.h>
int main(int argc, const char *argv[])
{
	FILE *fp = fopen("1.txt","r");
	char str1[100];
	char *str = fgets(str1, 100, fp):
	printf("%s\n",str);
	return 0;
}
