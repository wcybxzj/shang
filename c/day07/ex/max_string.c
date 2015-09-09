#include <stdio.h>
#include <stdlib.h>

int input_str(char **p, int num)
{
	int i, j;
	for (i = 0; i < num; i++) {
		p[i] = NULL;
	}

	for (j = 0; j < num; j++) {
		printf("input %d string: \n", j+1);
		i = 0;
		while (1) {
			p[j] = realloc(p[j], (i+1)*sizeof(char));
			if (p[j] == NULL) {
				return -1;
			}
			p[j][i] = getchar();
			if (p[j][i] == '\n') {
				p[j][i] = '\0';
				break;
			}
			i++;
		}
	}
	return 0;
}

//str1 abcdef
//str2 baEa
//result bbEcdef
int main(void)
{
	char *arr[2] = {NULL, NULL};	
	char *max = NULL;
	
	input_str(arr, 2);

	puts(arr[0]);
	puts(arr[1]);


	//max_str(arr, 2, &max);

	//puts(max);

	//free();

	return 0;
}

