#include <stdio.h>
#include <stdlib.h>

static void f1(void *p)
{
	printf("f1():%s\n",p);
}
static void f2(void *p)
{
    printf("f2():%s\n",p);
}

int main()
{// Begin!End!..bbb...aaa..ccc...............
	int job1,job2,job3;

	puts("Begin!");

	job1 = at_addjob(5,f1,"aaa");	
	if(job1 < 0)
	{
		fprintf(stderr,"at_addjob():%s\n",strerror(-job1));
		exit(1);
	}
	at_addjob_repeat(2,f2,"bbb");

	at_addjob(7,f1,"ccc");		
	
	puts("End!");

	while(1)
	{
		write(1,".",1);
		sleep(1);
	}

	exit(0);

}

