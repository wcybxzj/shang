#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM		 4	
#define NAMESIZE 32

void caculate(int argc, char **argv);
int math_main(int argc, char *argv[]);
void parse(char *p);

int addd(int a, int b) {return a+b;}
int subb(int a, int b) {return a-b;}
int mull(int a, int b) {return a*b;}
int divv(int a, int b) {return a/b;}

struct oper_st{
	char name[NAMESIZE];
	int(*p)(int, int);
	char op;
}arr[NUM] = {
	{"add", addd, '+'},
	{"sub", subb, '-'},
	{"mul", mull, '*'},
	{"div", divv, '/'}	
}; 

static int(*pf)(int, int);
static char opch;

int main(void)
{
	char *line = NULL;
	size_t num = 0;	

	while (1) {
		printf("[classs@shell]$ ");

		getline(&line, &num, stdin);	
		line[strlen(line)-1] = '\0';
		if (*line == '\0')
			break;

		parse(line);
	}

	free(line);
	return 0;
}

void parse(char *p)
{
	int argc = 0;
	char *argv[8] = {};	
	int flag = 0;

	while (*p) {
		if (*p == ' ' && flag == 0) {
			*p = '\0';
		}else if(*p != ' ' && flag == 0) {
			argv[argc++] = p;
			if (argc == 7)
				break;
			flag = 1;
		}else if (*p == ' ' && flag == 1) {
			*p = '\0';
			flag = 0;
		}
		p++;
	}
	if (!strcmp(argv[0], "exit"))
		exit(0);
	
	math_main(argc, argv);	
}

int math_main(int argc, char *argv[])
{
	int i;	
	
	for (i = 0; i < NUM; i++) {
		if (!strcmp(arr[i].name, argv[0])) {
			pf = arr[i].p;
			opch = arr[i].op;
		}
	}
	caculate(argc, argv);

	return 0;
}

void caculate(int argc, char **argv)
{
	printf("%d %c %d = %d\n", atoi(argv[1]), opch, atoi(argv[2]),\
			pf(atoi(argv[1]), atoi(argv[2])));	
}

