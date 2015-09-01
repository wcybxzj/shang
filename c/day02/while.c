#include <stdio.h>
#if 0
	while () {
	
	}
死循环：
	while (1);
	for( ; 1 ; );
break 和 continue;
break跳出整个循环
continue结束本次循环继续下一次循环
#endif
int main(void)
{
	int i;

	i = 0;
	while (i < 10) {
		if (i % 2 == 0) {
			i++;
		//	continue;
			break;
		}
		printf("%d ", i);
		i++;
	}	
	printf("\n");

	return 0;
}
