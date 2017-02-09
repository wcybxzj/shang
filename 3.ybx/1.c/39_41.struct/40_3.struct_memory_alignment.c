#include <stdio.h>
#include <stdlib.h>
struct{
    int a;
    char b;
    int c;
}t={ 10, 'C', 20 };

//通过打印地址观察结构体对齐
//./40_3.struct_memory_alignment 
//length: 12
//&a: 60091C
//&b: 600920
//&c: 600924
//
//地址是字节的编号
//0x1C->0x20 是4个地址编号对应4个字节
//0x20->0x24 是4个地址编号对应4个字节
int main(){
    printf("length: %d\n", sizeof(t));
    printf("&a: %X\n&b: %X\n&c: %X\n", &t.a, &t.b, &t.c);
    system("pause");
    return 0;
}
