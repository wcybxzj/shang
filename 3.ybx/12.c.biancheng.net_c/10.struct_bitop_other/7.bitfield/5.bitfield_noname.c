#include <stdio.h>
//无名位域
//位域成员可以没有名称，只给出数据类型和位宽, 不能使用

//无名位域一般用来作填充或者调整成员位置。因为没有名称，无名位域不能使用。

//如果没有位宽为 20 的无名成员，m、n 将会挨着存储，sizeof(struct bs) 的结果为 4；
////有了这 20 位作为填充，m、n 将分开存储，sizeof(struct bs) 的结果为 8。
int main(){
	struct bs{
	    int m: 12;
	    int  : 20;  //该位域成员不能使用
	    int n: 4;
	};
    printf("%d\n", sizeof(struct bs));
    return 0;
}
