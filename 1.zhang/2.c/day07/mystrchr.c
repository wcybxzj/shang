#include <stdio.h>
#include <stdlib.h>

char *mystrchr(const char *s, int c);

char *get_string(void)
{
	char *str = NULL;
	int i;

	i = 0;
	while (1) {
		str = realloc(str, sizeof(char)*(i+1));
		if (NULL == str) {
			break;
		}
		str[i] = getchar();
		if (str[i] == '\n') {
			str[i] = '\0';
			break;		
		}
		i++;
	}
		
	return str;
}

int main(void)
{
	char *p = NULL;
	int ch;
	char *ret;
		
	printf("input a string\n");
	p = get_string();		
	printf("input a charactor\n");
	ch = getchar();

	if ((ret = mystrchr(p, ch)) == NULL) {
		printf("%c is not in %s\n", ch, p);
	} else {
		printf("%c the first %s: %s\n", ch, p, ret);
	}

	free(p);

	return 0;
}

char *mystrchr(const char *s, int c)
{
	while (*s) {
		if (*s == c)
			return (char *)s;
		else 
			s++;
	}

	return NULL;
}

