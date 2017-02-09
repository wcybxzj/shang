#include <stdio.h>
//结构体位于相邻成员的类型相同的情况:
//如果它们的位宽之和小于类型的 sizeof 大小，
//那么后面的成员紧邻前一个成员存储，直到不能容纳为止；
//如果它们的位宽之和大于类型的 sizeof 大小，那么后面的成员将从新的存储单元开始，
//其偏移量为类型大小的整数倍。

//m、n、p 的类型都是 unsigned int,sizeof 的结果为 4 个字节（Byte）,也即 32 个位（Bit）。
//m、n、p 的位宽之和为 6+12+4 = 22，小于 32，所以它们会挨着存储，中间没有缝隙。
//sizeof(struct bs) 的大小之所以为 4，而不是 3，
//是因为要将内存对齐到 4 个字节，以便提高存取效率
//结构体内存对齐(查看1.c/8.structural_type/39.struct/40_2.struct_memory_alignment.c)
void test1()
{
	//6+12+4=22 不足1个int 就是32位->4字节
    struct bs{
	        unsigned m: 6;
	        unsigned n: 12;
	        unsigned p: 4;
	};

    printf("%d\n", sizeof(struct bs));//4
}

//将成员 m 的位宽改为 22，那么输出结果将会是 8，因为 22+12 = 38，大于 32，
//n 会从新的位置开始存储，相对 m 的偏移量是 sizeof(unsigned int)，也即 4 个字节。
void test2()
{
	//22+12+4=38 超过1个int 就是64位->8字节
    struct bs{
	        unsigned m: 22;
	        unsigned n: 12;
	        unsigned p: 4;
	};
    printf("%d\n", sizeof(struct bs));//8
}

int main(){
	test1();
	test2();
    return 0;
}
