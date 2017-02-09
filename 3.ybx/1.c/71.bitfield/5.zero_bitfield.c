#include <stdio.h>
#include <string.h>

#define PRINT_D(intValue)     printf(#intValue" is %d\n", (intValue));
#define OFFSET(struct, member)  ((char *)&((struct *)0)->member - (char *)0)

//http://www.cnblogs.com/luxiaoxun/archive/2012/11/09/2762438.html

//Q：关于位域的问题，空域到底表示什么？
//A：它表示之后的位域从新空间开始。

//sizeof(bit_info) is 8
//bit_info中的a, b占用4个字节的前4位，
//int:0 表示此时将填充余下所有没有填充的位，即4个字节余下的28位；
//int d:2; 将从第四个字节开始填充，又会占用4个字节，所以总大小为8.
typedef struct 
{
    int a : 1;
    int b : 3;
    int : 0;
    int d : 2;
}bit_info;

int main()
{
    PRINT_D(sizeof(bit_info))
    return 0;
}
