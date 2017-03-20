#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define evutil_offsetof(type, field) ((off_t)(&((type *)0)->field))

#define EVUTIL_UPCAST(ptr, type, field)                            \
       ((type *)(((char*)(ptr))- evutil_offsetof(type, field)))  

typedef struct Parent  
{  
	int a;
	int ch;
}P;  

int main(int argc, const char *argv[])
{
	off_t off= evutil_offsetof(P, ch);
	printf("%ld\n",off);//4

	P parent1;
	parent1.a = 123;
	parent1.ch = 456;
	int *ch_p = &parent1.ch;

	P* p1 = EVUTIL_UPCAST(ch_p, P, ch);
	printf("%d\n", p1->a);//123
	printf("%d\n", p1->ch);//456

	return 0;
}
