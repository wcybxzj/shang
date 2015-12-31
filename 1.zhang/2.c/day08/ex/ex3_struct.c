#include <stdio.h>
#include <string.h>

struct stu{
	char name[32];
	int chinese;
	int math;
}

int main(int argc, const char *argv[])
{
	struct stu *p1[3] = NULL;

	p1 = malloc(sizeof(stu)*3);




	return 0;
}
