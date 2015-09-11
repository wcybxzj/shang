#include <stdio.h>
#include <stdlib.h>

void *my_bsearch(const void *key, const void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));

typedef struct stu_st {
	int id;
	char name[32];
	int ch;
	int eng;
}stu_t;

int  func(const void* data, const void *key)
{
	const stu_t *d = data;
	const int *k = key;
	return d->id - *k;
}

int main(void)
{
	stu_t arr[10] = {
		{1, "zhanghuan", 90, 99},
		{2, "sujunjie", 98, 77},
		{3, "liuyufeng", 99, 66},
		{4, "name4", 98, 89}
		{5, "zhangning5", 98, 89}
		{6, "zhangning6", 98, 89}
		{7, "zhangning7", 98, 89}
		{8, "zhangning8", 98, 89}
		{9, "zhangning9", 98, 89}
		{10, "zhangning10", 98, 89}
	};

	int id;
	stu_t *ret = NULL;
	key = 7;
	ret = my_bsearch(&id, arr, 10, sizeof(stu_t), func);
	if (NULL == ret ) {
		printf("not match\n");
	}else {
		printf("match! id:%d name:%s 中文:%d 英语:%d \n", ret->id, ret->name, ret->ch, ret->eng);
	}
	return 0;
}

void *my_bsearch(const void *key, const void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)){
	int start, end ,middle, ret;
	start = 0;
	end = nmemb - 1;

	while (start<=end) {
		middle = (start + end)/2;
		ret = compar();//TODO
	}

	return NULL;
}
