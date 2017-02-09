#include <stdio.h>
//当结构体相邻成员的类型不同时:
//不同的编译器有不同的实现方案，GCC 会压缩存储，而 VC/VS 不会。

//在 GCC 下的运行结果为 4，三个成员挨着存储；
//在 VC/VS 下的运行结果为 12，三个成员按照各自的类型存储（与不指定位宽时的存储方式相同）。

int main(){
    struct bs{
        unsigned m: 12;
        unsigned char ch: 4;
        unsigned p: 4;
    };
    printf("%d\n", sizeof(struct bs));//4

    return 0;
}
