#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>


  
void signal_handle(int sig)  
{  
    printf("catch1111 the sig %d\n", sig);  
}  

void signal_handle2(int sig)  
{  
    printf("catch222222 the sig %d\n", sig);  
}  
  

int main()  
{  
	printf("pid = %d\n", getpid()); 
    signal(SIGINT, signal_handle);  //
    signal(SIGINT, signal_handle2); //覆盖前一个起作用
    printf("pid = %d\n", getpid());  

	while (1) {
		pause();
	}
  
    return 0;  
}  
