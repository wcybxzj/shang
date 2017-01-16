#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <event3/event.h>
#include "mm-internal.h" //此头本应是libvent内部使用 在这是测试下
#include "log-internal.h" //此头本应是libvent内部使用 在这是测试下

int main(int argc, const char *argv[])
{
	//自定义内存分配函数,先用系统API测试下自定义API
	void *(*malloc_fn)(size_t sz) = malloc;
	void *(*realloc_fn)(void *ptr, size_t sz) = realloc;
	void (*free_fn)(void *ptr) = free;
	event_set_mem_functions(malloc_fn, realloc_fn, free_fn);

	//1.malloc+free
	int *p = mm_malloc(sizeof(int));
	if (p==NULL) {
		event_err(ENOMEM,"mm_malloc");
	}
	*p =123;
	printf("%d\n", *p);
	mm_free(p);

	//2.calloc
	int i;
	char*str = mm_calloc(10,sizeof(char));
	if (str == NULL) {
		event_err(ENOMEM,"mm_calloc");
	}
	for (i = 0; i < 10; i++) {
		str[i]= 'a'+i;
	}
	printf("%s\n",str);

	//3.realloc
	str = mm_realloc(str,20);
	if (str == NULL) {
		event_err(ENOMEM,"mm_realloc");
	}
	for (i = 0; i < 20; i++) {
		str[i]= 'a'+i;
	}
	printf("%s\n",str);
	mm_free(str);

	//4.strdup
	char* str1 = "theyareok";
	char* str2 = mm_strdup(str1);
	if (str2==NULL) {
		event_err(ENOMEM,"mm_strdup");
	}
	printf("%s\n", str2);

	//5.realloc(ptr, 0)（也就是当作free(ptr)处理）
	mm_realloc(str2, 0);

	//6.realloc(NULL, sz)（也就是当作malloc(sz)处理）
	str = mm_realloc(NULL, sizeof(char));
	if (str==NULL) {
		event_err(ENOMEM,"mm_realloc");
	}
	*str = 'z';
	printf("%c\n", *str);
	return 0;
}
