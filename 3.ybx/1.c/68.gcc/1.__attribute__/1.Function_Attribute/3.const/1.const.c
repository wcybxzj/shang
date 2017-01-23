#include <stdio.h>
 
int  __attribute__((const)) inc100(int x)
{
    printf("%s(%d)\n", __FUNCTION__, x);
    return x + 1;
}
 
int  inc200(int x)
{
    printf("%s(%d)\n", __FUNCTION__, x);
    return x + 1;
}
 
/*
测试1:inc100 和 inc200都是正常运行2次并且返回2次结果
gcc 1.const.c 
./a.out 
inc100(10)
inc100(10)
11 11
inc200(10)
inc200(10)
11 11
]
*/

/*
测试2:inc100 只执行1次, 第二次不用执行而返回结果
gcc -O 1.const.c  或者 gcc -O1 1.const.c  (-O 和-O1都是按照优化级别1进行优化)
./a.out 
inc100(10)
11 11
inc200(10)
inc200(10)
11 11
]


*/

/*
分析原因:可以看到在优化的编译文件中inc100只调用了一次
gcc -S 1.const.c -o normal_const.s
gcc -O  -S 1.const.c -o optimise_const.s
*/
int main(void)
{
    int i, j;
 
    i = inc100(10);
    j = inc100(10);
 
    printf("%d %d\n", i, j);
 
    i = inc200(10);
    j = inc200(10);
 
    printf("%d %d\n", i, j);
 
    return 0;
}
