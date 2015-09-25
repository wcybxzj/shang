#include "../include/ini.h"
void * pthread_fun(void * p){
	unsigned char *c=p;
	while(1){
		*c=getchar();
		sleep(1);
	}

	return NULL;
}
