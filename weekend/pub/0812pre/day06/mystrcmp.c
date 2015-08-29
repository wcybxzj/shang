#include <stdio.h>

int mystrcmp(const char *s1, const char *s2);

int main(void)
{
	char *p = "aaaaaaaaaaaaaaaaaa";
	char *q = "b";
	int ret;

	ret = mystrcmp(p, q);
	if (ret > 0) {
		printf("%s is great than %s\n", p, q);
	} else if(ret < 0) {
		printf("%s is less than %s\n", p, q);
	} else {
		printf("%s is equal to %s\n", p, q);
	} 

	return 0;
}

int mystrcmp(const char *s1, const char *s2)
{
	while (*s1 || *s2) {
		if (*s1!=*s2) {
			return *s1-*s2;
		}		
		s1++;
		s2++;
	}

	return 0;
}

