#include <stdio.h>
#include <string.h>
int main(int argc, const char *argv[])
{
	struct stu{
		int id;
		char name[20];
	};

	struct stu s1;
	struct stu s2;

	s1.id=123;
	snprintf(s1.name, 20, "name is %d", 456);

	s2 = s1;

	printf("%d\n", s2.id);
	printf("%s\n", s2.name);

	return 0;
}
