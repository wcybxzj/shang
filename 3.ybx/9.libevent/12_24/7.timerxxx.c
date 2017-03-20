#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main(int argc, const char *argv[])
{
	struct timeval *tvp, *tvp1, *tvp2;
	tvp = malloc(sizeof(struct timeval));
	tvp1 = malloc(sizeof(struct timeval));
	tvp2 = malloc(sizeof(struct timeval));


	tvp->tv_sec = 123;
	tvp->tv_usec = 456;

	printf("%d\n",tvp->tv_sec);
	printf("%d\n",tvp->tv_usec);
	printf("================\n");
                     
	timerclear(tvp); 
	printf("%d\n",tvp->tv_sec);
	printf("%d\n",tvp->tv_usec);
	printf("================\n");

	tvp->tv_sec = 100;
	tvp->tv_usec = 400;

	tvp1->tv_sec = 200;
	tvp1->tv_usec = 500;

	timeradd(tvp, tvp1, tvp2);
	printf("%d\n",tvp2->tv_sec);//300
	printf("%d\n",tvp2->tv_usec);//900
	printf("================\n");

	return 0;
}
