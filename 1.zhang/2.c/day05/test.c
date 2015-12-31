#include <stdio.h>

void c(void)
{
	printf("c() is starting...\n");
	printf("c() is return...\n");
}
void b(void)
{
	printf("b() is starting...\n");
	c();
	printf("b() is ending...\n");
}

void a(void)
{
	printf("a() is starting...\n");
	b();
	printf("a() is ending...\n");
}

int main(void)
{
	printf("main is starting...\n");
	a();
	printf("main is ending....\n");

	return 0;
}
