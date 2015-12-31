#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main()
{

#pragma omp parallel sections
{
#pragma omp section	
	printf("[%d]Hello\n",omp_get_thread_num());
#pragma omp section
	printf("[%d]World\n",omp_get_thread_num());
}
	exit(0);
}


