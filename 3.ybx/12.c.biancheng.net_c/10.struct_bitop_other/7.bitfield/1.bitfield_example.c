#include <stdio.h>

//1.资料:
//http://c.biancheng.net/cpp/html/102.html

//2.输出:
//对于 n 和 ch，第一行输出的数据是完整的，第二行输出的数据是残缺的。
//./1.bitfield 
//0xad, 0xe, $
//0xb8901c, 0xd, :

//3.分析:
//第一次输出时，n、ch 的值分别是 0xE、0x24（'$' 对应的 ASCII 码为 0x24），
//换算成二进制是 1110、10 0100，都没有超出限定的位数，能够正常输出。

//第二次输出时，n、ch 的值变为 0x2d、0x7a（'z' 对应的 ASCII 码为 0x7a），
//换算成二进制分别是 101101、1111010，都超出了限定的位数。
//超出部分被直接截去，剩下 1101、111010，换算成十六进制为 0xd、0x3a（0x3a 对应的字符是 :）

//4.位域讲解
//C语言标准规定，位域的宽度不能超过它所依附的数据类型的长度。
//，成员变量都是有类型的，这个类型限制了成员变量的最大长度，:后面的数字不能超过这个长度。

//我们可以这样认为，位域技术就是在成员变量所占用的内存中选出一部分位宽来存储数据。

//C语言标准还规定，只有有限的几种数据类型可以用于位域。
//在 ANSI C 中，这几种数据类型是 int、signed int 和 unsigned int（int 默认就是 signed int）
//到了 C99，_Bool 也被支持了。

//编译器在具体实现时都进行了扩展，
//额外支持了 char、signed char、unsigned char 以及 enum 类型，
//所以上面的代码虽然不符合C语言标准，但它依然能够被编译器支持。

//
int main(){
    struct bs{
        unsigned m;//unsigned int 4字节 32位
        unsigned n: 4;//4位
        unsigned char ch: 6;//6位
    } a = { 0xad, 0xE, '$'};
    //第一次输出
    printf("%#x, %#x, %c\n", a.m, a.n, a.ch);

	//第二次输出
    //更改值后再次输出
    a.m = 0xb8901c;
    a.n = 0x2d;//warning: large integer implicitly truncated to unsigned type
    a.ch = 'z';//warning: large integer implicitly truncated to unsigned type
    printf("%#x, %#x, %c\n", a.m, a.n, a.ch);
    return 0;
}
