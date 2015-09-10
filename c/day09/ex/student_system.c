#include <stdio.h>
#include <stdlib.h>
#define NUM 4

void parse(char *p);

typedef struct stu_st {
	int id;
	char name[32];
	int grade;
}stu_p;

struct oper_st{
	char name[NAMESIZE];
	int(*p)(int, int);
}arr[NUM] = {
	{"add", add},
	{"add", add},
	{"add", add},
	{"displayall",displayall},
}; 

void add()
{
	/* code */
}


void del()
{
	/* code */
}


void change()
{
	/* code */
}

void displayall()
{
	/* code */
}

int main(int argc, const char *argv[])
{

	char *line = NULL;
	while (1) {
		printf("plz input (add 1 ybx 101|displayall|||)\n");

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
	int re = 0;
	if (!strcmp(argv[0], "add")) {
		pf = arr[i].p;
		re = pf(atoi(argv[1], argv[2],argv[3]));
		return re;
	}
	return 0;
}
