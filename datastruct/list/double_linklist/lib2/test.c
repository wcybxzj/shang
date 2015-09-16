#include <stdio.h>
#include <stdlib.h>
int main(int argc, const char *argv[])
{
	struct node_st {
		struct node_st *prev;
		struct node_st *next;
		char data[0];
	};

	printf("%d\n", sizeof(struct node_st));//16

	struct node_st *p = malloc(sizeof(struct node_st));
	printf("%d\n",p);// 20172816
	printf("%d\n",p->data);// 20172832

	return 0;
}
