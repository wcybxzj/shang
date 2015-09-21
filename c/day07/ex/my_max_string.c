#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int input_string(char **arr, int num);
char *max_string(char **arr, char **max);

int main(int argc, const char *argv[])
{
	char *arr[2] = {NULL, NULL};
	char *max;
	input_string(arr, 2);
	puts(arr[0]);
	puts(arr[1]);

	printf("-----------------------\n");
	printf("%s\n",max_string(arr, &max));
	printf("=======================\n");
	printf("%s\n",max);

	return 0;
}


int input_string(char **arr, int num)
{
	int i, j;
	for (i = 0; i < num; i++) {
		*(arr+i) = NULL;
	}

	for (i = 0; i < num; i++) {
		printf("plz enter string %d\n", i);
		for (j = 0; ; j++) {
			*(arr+i) = realloc(*(arr+i), (j+1)*sizeof(char));
			if (NULL == *(arr+i)) {
				return -1;
			}
			*(*(arr+i)+j) = getchar();
			if(*(*(arr+i)+j) == '\n') {
				*(*(arr+i)+j) = '\0';
				break;
			}
		}
	}

	return 0;
}

char *max_string(char **arr, char **max)
{
	char *p1 = arr[0];
	char *p2 = arr[1];
	char *str;
	int len = (strlen(p1) > strlen(p2))? strlen(p1) : strlen(p2);
	len++;
	str = malloc(len);
	*max = str;
	int move = 0;
	while (*p1||*p2) {
		move++;
		*str++ = (*p1>*p2)?*p1:*p2;
		if (*p1) {
			p1++;
		}
		if (*p2) {
			p2++;
		}
	}
	*str ='\0';
	str = str-move;
	return str;
}
