#include <stdio.h>
#include <stdlib.h>

//解决方法1:
//#ifdef __GNUC__                 /* Prevent 'gcc -Wall' complaining  */
//__attribute__ ((__noreturn__))  /* if we call this function as last */
//#endif

//解决方法2:
extern void exitnow() __attribute__((noreturn));

void exitnow(){
	exit(1);
}

int foo(int n)
{
	if ( n > 0 )
	{
		printf("n is bigger than zero\n");
		exitnow();
		/* control never reaches this point */
	}
	else
		return 0;
}

/*
//报错
[root@web11 1.format]# gcc -Wall 2.noreturn.c
2.noreturn.c: In function ‘foo’:
2.noreturn.c:18: warning: control reaches end of non-void function
*/
int main(int argc, const char *argv[])
{
	foo(1);
	return 0;
}
