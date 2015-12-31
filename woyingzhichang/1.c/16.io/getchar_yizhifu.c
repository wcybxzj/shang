#include <stdio.h>

//11
//a
//11 97
int main(int argc, const char *argv[])
{
	char ch;
	int i;
	scanf("%d",&i);
	
	//getchar();
	//scanf("%c",&ch);

	//效果同上
	scanf("%*c%c", &ch);

	printf("%d %d\n", i, ch);

	return 0;
}
