#include <stdio.h>
#include <string.h>

struct stu{
	int id;
	char name[32];
	int grade;
};

void func(struct stu s1)
{
	strcpy(s1.name, "www");
}

int main(int argc, const char *argv[])
{


	struct stu s;
	s.id = 123;
	strcpy(s.name, "ybx");
	s.grade =456;

	func(s);

	printf("%s\n", s.name);

	return 0;
}
