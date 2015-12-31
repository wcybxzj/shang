#include <stdio.h>
#include <stdlib.h>

#define ERR(fun) \
		do {\
			fprintf(stderr,"%d: %s is error\n", __LINE__, #fun);\
			exit(EXIT_FAILURE);\
		}while(0)

int main(void)
{
	ERR(open());

	return 0;
}
