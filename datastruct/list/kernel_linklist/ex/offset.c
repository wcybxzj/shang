#include <stdio.h>

struct list_head{
	struct list_head *prev;
	struct list_head *next;
};

//struct test_st {
//	int id;
//	int age;
//	char sex;
//	struct list_head node;
//}__attribute__((packed));

struct test_st {
	int id;
	int age;
	char sex;
	struct list_head node;
};

int main(void)
{
	printf("%d\n", sizeof(struct list_head));//16

	printf("%d\n", sizeof(struct test_st));//32

	struct test_st tmp;

	//&tmp = 1565935568
	//&tmp.node = 1565935584
	printf("&tmp = %d\n", (int)&tmp);	
	printf("&tmp.node = %d\n", (int)&tmp.node);
	printf("%d\n",(int)(&(((struct test_st *)0)->node)));//16
	printf("%d\n",(int)(&(((struct test_st *)1)->node)));//17

	return 0;
}
