#include <stdio.h>
int main(int argc, const char *argv[])
{
	int i;
	int ret;
	printf("请输入整型\n");
	//假如输入的不是整型例如一个 a 就会变成死循环
	//scanf要的是整型.给一个字符直接返回 0 (0个数据被正常赋予)
	while (1) {
		ret = scanf("%d", &i);
		if (ret == 0) {
			break;
		}
		printf("ret is %d\n", ret);
		printf("%d\n", i);
	}
	return 0;
}
