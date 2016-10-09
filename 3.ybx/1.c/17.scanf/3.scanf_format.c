#include <stdio.h>
int main(int argc, const char *argv[])
{
	int i;
	char ch;

	//问题:这有一个回车,如果不处理后边永远只能读这个换行
	scanf("%d", &i);

	//方法1:
	//getchar();

	//方法2:
	//使用抑制符 %*c的意思是抑制掉一个char
	scanf("%*c%c", &ch);

	printf("i %d , ch %d , ch %c\n", i , ch, ch);

	return 0;
}
