#include <stdio.h>
#include <stdlib.h>

//address:0xda7010 value:123
//address:0xda7010 value:456
//address:0xda7010 value:456
int main(void){
	int *p;
	int *p1=NULL;
	p = malloc(sizeof(*p));
	if (p == NULL) {
		perror("malloc():");
		exit(1);
	}

	*p=123;
	printf("address:%p value:%d\n", p, *p);

	free(p);
	//p=NULL;

	//free完全还可以使用
	*p=456;
	printf("address:%p value:%d\n", p, *p);

	//free后,再malloc和第一个malloc空间是相同的
	p1 = malloc(sizeof(*p1));
	printf("address:%p value:%d\n", p1, *p1);

	exit(0);
}
