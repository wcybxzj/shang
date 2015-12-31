#include <stdio.h>
#include <string.h>

char *my_strstr(char *s1, char *s2){
	int max = strlen(s2);
	char *tmp = NULL;
	char *s2_start = s2;
	int i = 1;
	while (*s1++) {
		if (i > 1 && *s2_start != *s1) {
			s1--;
		}

		if(*s2_start == *s1) {
			if (i == 1) {
				tmp = s1;
			}
			if (i==max) {
				return tmp;
			}
			i++;
			s2_start++;
		}else{
			i = 1;
			tmp = NULL;
			s2_start = s2;
		}
	}
	return tmp;
}


int main(int argc, const char *argv[])
{
	char *str1 = "this cocool is cool play";
	char *str2 = "cool";
	char *p1;
	char *p2;

	p1 = strstr(str1, str2);
	printf("%s\n", p1);

	printf("===========\n");

	p2 = my_strstr(str1, str2);
	printf("%s\n", p2);

	return 0;
}
