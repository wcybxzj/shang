#include <stdio.h>

int main(void)
{
	srand(time(NULL));
	char *arr[] = {
		"马伟涛", "孙月", "周超", "辛克乐", "张欢",
		"陈庆国", "刘宇峰", "董文凯", "万雪翔", "刘璐",
		"许俊杰", "于文章", "杨秉熙", "张跃龙", "徐乐",
		"屈东方", "王世界", "丁宏红", "朱建宁", "郭文"
	};
	int i, j;
	
	j = rand() % 20;

	for (i = 0; i < 20; i++) {
		if (i == j) {
			putchar('[');
		} else {
			putchar(' ');
		}	
		printf("%s%c",arr[i], i==j?']':' ');
		if ((i+1) % 5 == 0) 
			putchar('\n');
	}

	return 0;
}
