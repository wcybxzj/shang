#include <stdio.h>
#include <stdlib.h>
#define COUNT 10
void *my_bsearch_arr(const void *key, const void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));

typedef struct stu_st {
	int id;
	char name[32];
	int ch;
	int eng;
}stu_t;

int  func(const void *key, const void* data)
{
	const stu_t *d = data;
	const int *k = key;
	printf("*k %d\n", *k);
	printf("*d->id %d\n", d->id);
	return *k - d->id;
}

int main(void)
{
	stu_t arr[COUNT] = {
		{1, "zhanghuan", 90, 99},
		{2, "sujunjie", 98, 77},
		{3, "liuyufeng", 99, 66},
		{4, "name4", 98, 89},
		{5, "zhangning5", 98, 89},
		{6, "zhangning6", 98, 89},
		{7, "zhangning7", 98, 89},
		{8, "zhangning8", 98, 89},
		{9, "zhangning9", 98, 89},
		{10, "zhangning10", 98, 89}
	};

	stu_t *ret = NULL;
	int key = 8;
	ret = my_bsearch_arr(&key, arr, COUNT, sizeof(stu_t), func);
	if (NULL == ret ) {
		printf("not match\n");
	}else {
		printf("match! id:%d name:%s 中文:%d 英语:%d \n", ret->id, ret->name, ret->ch, ret->eng);
	}
	return 0;
}

void *my_bsearch_arr(const void *key, const void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)){
	int start, end ,mid, ret;
	start = 0, end = nmemb - 1;

	while (start<=end) {
		mid = (start + end)/2;
		//printf("mid is %d\n", mid);
		ret = compar(key, base + mid * size);
		if (ret == 0) {
			return (void *)(base + mid * size);
		}
		if (ret > 0) {
			start = mid + 1;
		}else{
			end = mid - 1;
		}
	}

	return NULL;
}
