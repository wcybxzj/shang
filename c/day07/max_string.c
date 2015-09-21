#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int input_str(char **p, int num);
char *max_str(char **p, int num, char **max);

int main(void)
{
	char *arr[2] = {NULL, NULL};	
	char *max = NULL;
	int i;
	
	input_str(arr, 2);

	puts(arr[0]);
	puts(arr[1]);

	max_str(arr, 2, &max);


	puts(max);

	free(arr[0]);
	free(arr[1]);

	free(max);
	return 0;
}

int input_str(char **p, int num)
{
	int i, j;	

	for (j = 0; j < num; j++) {
		p[j] = NULL;
	} 

	for (j = 0; j < num; j++) {
		printf("input %d string: ", j+1);
		i = 0;
		while (1) {
			p[j] = realloc(p[j], (i+1)*sizeof(char));	
			if (NULL == p[j]) 
				return -1;
			*(p[j]+i)= getchar();			
			if (p[j][i] == '\n') {
				p[j][i] = '\0';
				break;
			}
			i++;
		}
	}

	return 0;
}

char *max_str(char **p, int num, char **max)
{
	int len;
	char *tmp;
	char *str1 = p[0];
	char *str2 = p[1];

	len = (strlen(p[0]) > strlen(p[1]) ? strlen(p[0]) : strlen(p[1])) + 1;
	
	*max = malloc(len);
	if (NULL == *max) {
		return NULL;
	}
	memset(*max, 0x00, len);
	tmp = *max;	
	
	while (*str1 || *str2) {
		*tmp++ = *str1 > *str2 ? *str1:*str2;
		if (*str1) 
			str1 ++;
		if (*str2)
			str2 ++;
	}
	*tmp = '\0';

	return *max;
}

