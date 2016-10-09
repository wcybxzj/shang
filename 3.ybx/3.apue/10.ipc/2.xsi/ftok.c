#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>


#define NUM1 0x00000010
#define NUM2 0x22222210
//证明proj_id只有最后8位1个字节有效果
int main(void){
	key_t k1, k2;
	k1 = ftok("/etc/hosts", NUM1);
	k2 = ftok("/etc/hosts", NUM2);

	printf("%d\n", k1);
	printf("%d\n", k2);

	exit(0);
}
