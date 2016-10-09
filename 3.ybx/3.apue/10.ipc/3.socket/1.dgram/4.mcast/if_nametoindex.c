#include <stdio.h>
#include <stdlib.h>
#include <net/if.h>

int main(void){
	printf("eth0:%d\n", if_nametoindex("eth0"));
	printf("eth1:%d\n", if_nametoindex("eth1"));
	printf("eth2:%d\n", if_nametoindex("eth2"));
	printf("br0:%d\n", if_nametoindex("br0"));
	exit(0);
}
