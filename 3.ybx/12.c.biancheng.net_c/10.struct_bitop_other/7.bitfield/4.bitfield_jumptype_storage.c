#include <stdio.h>
//结构体成员之间穿插着非位域成员，那么不会进行压缩
//在各个编译器下 sizeof 的结果都是 12。

//通过上面的分析，我们发现位域成员往往不占用完整的字节，有时候也不处于字节的开头位置，
//因此使用&获取位域成员的地址是没有意义的，
//C语言也禁止这样做。地址是字节（Byte）的编号，而不是位（Bit）的编号。
int main(){
	struct bs{
		unsigned m: 12;
		unsigned ch;
		unsigned p: 4;
	};

	printf("%d\n", sizeof(struct bs));//12
	return 0;
}
