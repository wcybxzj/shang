#include <stdio.h>

#define SIZE 32

int mystrcmp(const char *s1, const char *s2);

int main(void)
{
	char dest[SIZE] = "za is too ";
	char src[] = "yt is too ";

	printf("%d\n", mystrcmp(dest, src));

	return 0;
}

int mystrcmp(const char *s1, const char *s2)
{
	while (*s1 || *s2) {
		if (*s1 != *s2) {
			return *s1 - *s2;
		}else {
			s1++;
			s2++;
		}
	}	

	return 0;
}

