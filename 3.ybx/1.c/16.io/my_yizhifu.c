#include <stdio.h>

//加入输入1.1,程序死循环
//办法是对scanf返回值做判断
int main(int argc, const char *argv[])
{
	int i;
	int ret;
	while (1) {
		ret = scanf("%d",&i);
		//解决方案
		if (ret == 0) {
			break;
		}
		printf("%d\n", i);
	}
	return 0;
}
