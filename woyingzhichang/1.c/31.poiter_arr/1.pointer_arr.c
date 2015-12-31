#include <stdio.h>
#include <stdlib.h>
int main(int argc, const char *argv[])
{
	int i, j, k;
	char *name[5] = {"Iam", "the", "bset", "a in the", "c world"};
	char *tmp;

	//printf("%d\n",sizeof(name));//20
	//printf("%d\n",sizeof(name[0]));//4

	for (i = 0; i < 5-1; i++) {
		k = i;
		for (j = k+1; j < 5; j++) {
			if(strcmp(name[k], name[j]) > 0) {
				k = j;
			}
		}
		if (k != i) {
			tmp = name[i];
			name[i] = name[k];
			name[k] = tmp;
		}
	}

	for (i = 0; i < sizeof(name)/sizeof(name[0]); i++) {
		puts(name[i]);
	}

	return 0;
}
