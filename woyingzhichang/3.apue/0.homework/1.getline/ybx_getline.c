#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define LEN 120

ssize_t ybx_getline(char **lineptr, size_t *n, FILE *stream);
void test_ybx_getline();
void test_getline();

int main(int argc, const char *argv[])
{
	printf("test getline\n");
	test_getline();
	printf("--------------------------------------\n");
	printf("test ybx_getline\n");
	test_ybx_getline();
	exit(0);
}

ssize_t ybx_getline(char **lineptr, size_t *n, FILE *stream){
	if (NULL == *lineptr) {
		*lineptr = malloc(LEN);
		*n = LEN; 
	}

	int tmp, ret = 0;
	while (1) {
		tmp = fgetc(stream);
		if (EOF == tmp) {
			return -1;
		}
		ret++;
		if ('\n'==tmp) {
			break;
		}
	}

	while (1) {
		if (ret < *n) {
			fseek(stream, -ret, SEEK_CUR);
			fgets(*lineptr, *n, stream);
			return ret;
		}else{
			*n = *n+LEN;
			*lineptr = realloc(*lineptr, *n);
		}
	}

	return ret;
}

void test_getline()
{
	FILE *fp;
	char *str;
	int num;
	int read;

	fp = fopen("services", "r");
	if (NULL == fp) {
		perror("fopen():");
		exit(1);
	}
	str = NULL;
	while ((read = getline(&str, &num, fp))>=0) {
		printf("content:%s",str);
		printf("strlen(str)%d, read:%d, num: %d\n", strlen(str), read, num);
	}

	free(str);
	fclose(fp);
}

void test_ybx_getline()
{
	FILE *fp;
	char *str;
	int num;
	int read;

	fp = fopen("services", "r");
	if (NULL == fp) {
		perror("fopen():");
		exit(1);
	}
	str = NULL;
	while ((read = ybx_getline(&str, &num, fp))>=0) {
		printf("content:%s",str);
		printf("strlen(str)%d, read:%d, num: %d\n", strlen(str), read, num);
	}

	free(str);
	fclose(fp);
}
