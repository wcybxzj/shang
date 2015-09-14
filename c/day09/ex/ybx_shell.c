#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM 4
#define NAMESIZE 32

void parse(char *p);
void calculate(char *argv[]);

int add1(int a, int b) { return a + b; }
int sub1(int a, int b) { return a - b; }
int mul1(int a, int b) { return a * b; }
int div1(int a, int b) { return a / b; }

struct op_st{
	char name[NAMESIZE];
	int (*func)(int, int);
	char op;
} arr[NUM] = {
	{"add", add1, '+'},
	{"sub", sub1, '-'},
	{"mul", mul1, '*'},
	{"div", div1, '/'}
};


int main(int argc, const char *argv[])
{	
	char *line = NULL;
	size_t linecap = 0;
	ssize_t linelen;

	while (1) {
		printf("ybx$");
		linelen = getline(&line, &linecap, stdin);
		if (linelen == 1 && line[0]=='\n') {
			continue;
		}
		line[linelen-1] ='\0';
		parse(line);
		line = NULL;
	}

	free(line);
	return 0;
}

void parse(char *p)
{
	int i;
	int argc = 0;
	int flag = 0;
	char *argv[8] = {};
	while (*p) {
		if (argc==8) {
			break;
		}
		if (flag==0 && *p != ' ') {
			argv[argc++] = p;
			flag = 1;
		}else if(flag==1 && *p == ' '){
			*p = '\0';
			flag = 0;
		}else if(flag==0 && *p == ' '){
			*p = '\0';
		}
		p++;
	}

	if (!strcmp(argv[0],"exit")){
			exit(1);
	}
	calculate(argv);
}

void calculate(char *argv[])
{
	int i;
	for (i = 0; i < NUM; i++) {
		if ( !strcmp(arr[i].name, argv[0]) ) {
			printf("%s %c %s = %d\n",
					argv[1], arr[i].op ,argv[2], arr[i].func(atoi(argv[1]), atoi(argv[2])));
		}
	}
}
