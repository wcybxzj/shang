#include <stdio.h>
//http://www.cnblogs.com/sunp823/p/5601433.html

//结构体对齐规则:
//一般情况下，即没有#pragma pack宏定义和使用位域的情况下，结构体对齐一般满足三个原则：
//1.普通数据成员：第一个数据成员放在offset为0的地方，以后每个数据成员存储的起始位置要从该成员大小的整数倍开始（比如int为4字节，则要从4的整数倍地址开始存储）。
//2.结构体数据成员：如果一个结构里有某些结构体成员，则结构体成员要从其内部最大元素大小的整数倍地址开始存储。
//（struct a里存有struct b，b里有char1字节，int4字节，double8字节等元素，那b应该从8的整数倍开始存储）。
//3.结构体的总大小，必须是其内部最大普通元素占据字节的整数倍。

//位域合并规则:
//1. 如果相邻位域类型相同，位宽之和小于类型的sizeof大小,则后面的字段紧邻前一个字段存储，直到不能容纳为止；
//2. 如果相邻位域类型相同，位宽之和大于类型的sizeof大小，则后面的字段将从新的存储单元开始，其偏移量为其类型大小的整数倍；
//3. 如果相邻位域类型不同，则vc6采取不压缩方式，dev-c++ 和GCC都采取压缩方式。
//依然满足结构体内存对齐三个原则中的原则1，
//在不压缩方式下，如果前一个位域类型有填充，后面的位域类型和前面的位域类型不相同，则填充的区域不能存放放后面的位域，需另开辟空间；
//而在压缩方式下，填充的区域如果可以放下后者位域，则存放，放不下的情况下再另开辟空间。
//4.一个位域必须存储在同一个字节中，不能跨字节；
//5.如一个字节所剩空间不够存放另一位域时，应从下一单元起存放该位域。也可以有意使某位域从下一单元开始。

void main(int argn ,char *argv)
{
	//unsigned 的意思是unsigned int
    struct test {
        unsigned a:10;
        unsigned b:10;
        unsigned c:6;
        unsigned :2;
        unsigned d:4;
	}data,*pData;
    data.a=0x177;
    data.b=0x111;
    data.c=0x7;
    data.d=0x8;

	//0.
	printf("%d\n",sizeof(unsigned));//4
	printf("%d\n",sizeof(data));//4
	printf("==========================================\n");

    //1.位域可以使用指针获取值,但是不能对位域取地址
    pData=&data;
    printf("data.a=%x data.b= %x data.c=%x data.d=%x\n", 
		pData->a,pData->b,pData->c,pData->d);
	printf("==========================================\n");

	//2.位域不能取地址
	//printf("%x\n", &data.a);//error: cannot take address of bit-field ‘a’

	//3.位域规则1
    printf("sizeof(data)=%d\n",sizeof(data));   //4 bytes
	printf("==========================================\n");

	//4.位域规则2
    struct testLen{
		char a:5; //1个字节
		char b:5; //1个字节
		char c:5; //1个字节
		char d:5; //1个字节
		char e:5; //1个字节
    }len;
    printf("sizeof(len)=%d\n",sizeof(len));     //5bytes
	printf("==========================================\n");


	//5.位域规则2
    struct testLen1{
        char a:5;
        char b:2;//1个字节
        char d:3;
        char c:2;//1个字节
        char e:7;//1个字节
        }len1;
    printf("sizeof(len1) =%d\n",sizeof(len1));    //3bytes
	printf("==========================================\n");

	//难点
	//6.位域规则3+结构体对齐规则
	//gcc一个结构体内的,位域不同类型是进行压缩的
	//gcc压缩条件下:char类型占据一个字节，而a占用其中两位；
	//根据结构体对齐原则1，b的类型int占据四个字节，应该从4的整数倍处开始存放，
	//所以char后应该填充三个字节，这三个字节能够容纳b和c，因此不需要重新开辟空间
	//，直接在这三个字节上存储，所以sizeof(A) = 4;
	typedef struct {
		char a:2;//扩充3个字节 成为4个字节,32位-2=30为空闲
		int b:15;
		int c:15;
	} A;
	printf("sizeof(A)=%d\n",sizeof(A));//4
	printf("==========================================\n");

	//7.情况和6类似 唯一的区别b是16
	//a+b=18, c需要单个存到1个int
	typedef struct {
		char a:2;//扩充3个字节 成为4个字节,32位-2=30为空闲
		int b:16;
		int c:15;
	} A1;
	printf("sizeof(A1)=%d\n",sizeof(A1));//8
	printf("==========================================\n");

	//8.位域规则3+结构体对齐规则
	//a+b=32bit c单独存在一个char
	//本来是5个字节 然后结构体对齐5+3=8
	typedef struct {
		int a:30;
		char b:2;
		char c:1;
	} A2;
	printf("sizeof(A2)=%d\n",sizeof(A2));//8
	printf("==========================================\n");

	//9.位域规则3+结构体对齐规则
	///a+匿名位域=个字节
	//b是1个字节,b+6字节才能让long d存储进来
	//7字节是56位,已经用了7位,剩余56-7=49位,
	//正好能存储d和e到b的扩展空间中
	printf("long:%d\n", sizeof(long));//8
    struct testLen2{
        char a:2;
        char :3;
        char b:7;
        long d:45;
        char e:4;
        }len2;
    printf("sizeof(len2)=%d\n",sizeof(len2));  //8
	printf("==========================================\n");

	//10.同9区别是d是46位
	//d能存进去b,b剩余49-46=3位,
	//e就存不到b需要单独存到一个char
	//a+匿名位域是1个字节 
	//b是1+6=7字节
	//d压缩存出b中
	//e是1个字节
	//结果9个字节,最大元素是long,结构提对齐规则3,最后是16字节
    struct testLen3{
        char a:2;
        char :3;
        char b:7;
        long d:46;
        char e:4;
        }len3;
    printf("sizeof(len3)=%d\n",sizeof(len3));//16

}
