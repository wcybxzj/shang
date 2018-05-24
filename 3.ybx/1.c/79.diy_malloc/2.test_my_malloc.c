#include "my_malloc.h"

typedef struct info* info_t;
struct  info{
	char name[20];
	int age;
};

void test_malloc()
{
	info_t info_p = NULL;
	info_p = my_malloc(sizeof(struct info));
	if (info_p == NULL) {
		printf("my_malloc error\n");
		exit(1);
	}
	strcpy(info_p->name,"dabing");
	info_p->age=20;
	printf("name:%s\n",info_p->name);
	printf("age:%d\n",info_p->age);
	my_free(info_p);
}

void test_calloc()
{
	int i;
	int num =10;
	size_t *arr =my_calloc(num , sizeof(size_t));
	if (!arr) {
		printf("calloc error\n");
		exit(1);
	}
	for (i = 0; i < num; i++) {
		arr[i]=i+100;
	}
	for (i = 0; i < num; i++) {
		printf("%d\n",arr[i]);
	}
	my_free(arr);
}

void test_realloc()
{
	int i;
	int num =10;
	size_t *arr = my_calloc(num , sizeof(size_t));//10*8=80
	if (!arr) {
		printf("calloc error\n");
		exit(1);
	}
	for (i = 0; i < num; i++) {
		arr[i]=i+100;
	}

	int num2 = 5;
	size_t *arr2  = my_realloc(arr, num2*sizeof(size_t));
	if (!arr2) {
		printf("my_realloc() error\n");
		exit(1);
	}
	for (i = 0; i < num2; i++) {
		printf("%d\n",arr2[i]);
	}

	my_free(arr2);
}

void test_get_malloc_size()
{
	void *ptr;
	ptr = my_malloc(20);
	printf("%d\n",get_malloc_size(ptr));//24
}

int main(int argc, const char *argv[])
{
	//test_malloc();
	//test_calloc();
	//test_realloc();
	test_get_malloc_size();

	return 0;
}
