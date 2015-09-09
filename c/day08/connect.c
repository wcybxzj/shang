#include <stdio.h>

#define TEST(cont) hello##cont()

void helloboys(void)
{
	printf("good boy\n");
}

void hellogirls(void)
{
	printf("beautiful girl\n");
}

int main(void)
{
	TEST(boys);

	TEST(girls);

	return 0;
}
