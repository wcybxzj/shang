#include <stdio.h>
#define EV_CHECK_FMT(a,b) __attribute__((format(printf, a, b)))
void event_err(int eval, const char *fmt, ...) EV_CHECK_FMT(2,3);
#undef EV_CHECK_FMT

//gcc -E 3.#undef.c
int main(int argc, const char *argv[])
{
	
	return 0;
}
