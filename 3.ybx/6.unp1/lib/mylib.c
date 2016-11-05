#include "mylib.h"

pid_t gettid() 
{ 
     return syscall(SYS_gettid); 
} 


//使用前必须先执行一次 srand(time(NULL));
int my_rand(int range){
	return rand()%range;
}

/*
int main()
{
	printf("tid:%d\n", gettid());
	printf("=====================\n");
	srand(time(NULL));
	for (i = 0; i < MAX; i++) {
		printf("%d\n",my_rand(10) );
	}
}
*/
