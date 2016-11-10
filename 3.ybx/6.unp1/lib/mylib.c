#include "mylib.h"

pid_t gettid() 
{ 
     return syscall(SYS_gettid); 
} 


//使用前必须先执行一次 srand(time(NULL));
int my_rand(int range){
	return rand()%range;
}

int setnonblocking( int fd ){
    int old_option = fcntl( fd, F_GETFL );
    int new_option = old_option | O_NONBLOCK;
    fcntl( fd, F_SETFL, new_option );
    return old_option;
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
