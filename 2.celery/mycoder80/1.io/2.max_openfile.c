#include<stdio.h>
#include<string.h>
//ulimit -a , open file - 3
int main(int argc, char *argv[]){
	int i;
	FILE *fp;
	while (1) {
		fp = fopen("index.txt", "r");
		if (fp!=NULL) {
			i++;
			printf("%d\n", i);
		}
		else{
			break;
		}
	}
	return 0;
}
