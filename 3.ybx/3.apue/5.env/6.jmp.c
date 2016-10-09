#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

static jmp_buf env;

void d()
{
	printf("start d\n");
	//longjmp(env, 123);
	longjmp(env, 0);
	printf("end of d\n");
}

void c()
{
	printf("start c\n");
	d();
	printf("end of c\n");
}

void b()
{
	printf("start b\n");
	c();
	printf("end of b\n");
}


void a()
{
	printf("start a\n");
	b();
	printf("end of a\n");
}


int main(int argc, const char *argv[])
{
	int ret;
	ret = setjmp(env);
	if (ret == 0) {
		a();
	}else{
		printf("ret %d\n", ret);
	}

	return 0;
}
