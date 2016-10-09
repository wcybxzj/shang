#include <stdio.h>
#include <stdlib.h>

struct student_st{
	int age;
};

struct student_list{
	int num;
	struct student_st *p;
};

int main(int argc, const char *argv[])
{
	int i;
	struct student_list list;
	list.num = 0;
	list.p = NULL;

	for (i = 0; i < 5; i++) {
		list.num++;
		list.p = realloc(list.p, list.num * sizeof(struct student_st));
		(list.p+i)->age = i;
	}

	for (i = 0; i < list.num; i++) {
		printf("%d\n", list.p[i].age);
	}

	return 0;
}
