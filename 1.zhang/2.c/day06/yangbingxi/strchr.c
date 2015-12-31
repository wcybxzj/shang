#include <stdio.h>
#include <string.h>

char *my_strchr(char *s, int c){
	char *p;
	//s不是\0
	while (*s) {
		if (*s==c) {
			return s;
		}
		s++;
	}
	return NULL;
}

int main(int argc, const char *argv[])
{
	char *s="Golden Global View";
	char *p;
	char *p1;

	p = strchr(s,'V');
	if(p)
		printf("%s",p);
	else
		printf("Not Found!");

	printf("\n=================\n");

	p1 = my_strchr(s,'V');
	if(p1)
		printf("%s",p1);
	else
		printf("Not Found!");

	printf("\n=================\n");
	return 0;
}
