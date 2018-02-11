#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//方法1:手写
void func1()
{
    int num, n, i = 0;
    char str[20], tmp[20];
    scanf("%d", &num);
    n = num % 10;
    while (n>0)
    {
        tmp[i++] = n + '0';
        num = (num - n) / 10;
        n = num % 10;
    }
    tmp[i] = '\0';
    for (i=0; i<=strlen(tmp)-1; i++)
    {
        str[i] = tmp[strlen(tmp)-i-1];
    }
    str[i] = '\0';
    printf("string is:%s\n", str);
    return;
}

//方法2:使用sprintf
void func2()
{
	int number = 429496729;
	char string[25];
	sprintf(string, "%d", number);

	printf("integer = %d string = %s\n", number, string);
}

int main()
{
	//func1();
	func2();
}
