#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct stu1{
	char name[32];
	int age;
};

struct people{
	struct stu1 s_data;
	int grade;
};


int main(int argc, const char *argv[])
{
	struct people *p = malloc(sizeof(struct people));
	strcpy(p->s_data.name, "ybx");
	p->grade = 201;
	p->s_data.age=30;


	printf("%d %d %s\n", p->grade, p->s_data.age, p->s_data.name);
	return 0;
}


