#include <stdio.h>

struct st1{
	int i;//4
	char ch;//1
	float f;//4
};

struct st2{
	int i;//4
	char ch;//1
	char ch1;//1
	float f;//4
};

struct st3{
	int i;//4
	char ch;//1
	float f;//4
	char ch1;//1
};

int main(int argc, const char *argv[])
{
	struct st1 s1;
	printf("%d\n", sizeof(s1));//12

	struct st2 s2;
	printf("%d\n", sizeof(s2));//12

	struct st3 s3;
	printf("%d\n", sizeof(s3));//16
}
