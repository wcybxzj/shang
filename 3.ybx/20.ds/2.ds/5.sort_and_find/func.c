#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void my_srand()
{
	srand((int)time(0));
}

//取值min->max范围内的数
int my_rand(int min, int max)
{
	return (rand()%(max-min+1))+min;
}

