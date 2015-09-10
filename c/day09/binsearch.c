#include <stdio.h>
#include <stdlib.h>

void *mybsearch(const void *key, const void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));

typedef struct stu_st {
	int id;
	char name[32];
	int ch;
	int eng;
}stu_t;

int cmpare(const void *data, const void *key)
{
	const stu_t *d = data;
	const int *k = key;
	return d->id - *k;	
}

int main(void)
{
	stu_t arr[] = {
		{1, "zhanghuan", 90, 99},
		{5, "sujunjie", 98, 77},
		{7, "liuyufeng", 99, 66},
		{10, "zhangning", 98, 89}
	};
	int id;
	stu_t *ret;

	id = 7;
	ret = (stu_t *)mybsearch(&id, arr, 4, sizeof(stu_t), cmpare);
	if (ret == NULL) {
		printf("not match\n");
	} else {
		printf("find it: %d %s %d %d\n", ret->id, ret->name,\
				ret->ch, ret->eng);
	}

	return 0;
}

void *mybsearch(const void *key, const void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *))
{
	int start, end;
	const char *tmp = base;
	int mid, ret;

	start = 0, end = nmemb - 1;

	while (start <= end) {
		mid = (start + end) / 2;
		ret = compar(key, tmp + mid * size);//TODO
		if (ret == 0) {
			return (void *)(tmp + mid * size);
		}	
		if (ret > 0) {
			start = mid + 1;
		}else {
			end = mid - 1;
		}
	}
	
	return NULL;
}

