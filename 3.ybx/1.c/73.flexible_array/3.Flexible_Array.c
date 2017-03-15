#include <stdlib.h>
#include <string.h>
 
//这种玩法英文叫：Flexible Array，中文翻译叫：柔性数组
//分配一个不定长的数组,length代表数组的长度，contents代表数组的内容。
//后面代码里的 this_length（长度是10）代表是我想分配的数据的长度。

//在输出*thisline时，我们发现其中的成员变量contents的地址居然和thisline是一样的（偏移量为0x0??!!）。
//但是当我们输出thisline->contents的时候，你又发现contents的地址是被offset了0x4了的，内容也变成了10个‘a’。
//（coolshell觉得这是一个GDB的bug，VC++的调试器就能很好的显示）

//sizeof(char[0])或是 sizeof(int[0]) 之类的零长度数组，你会发现sizeof返回了0，
//这就是说，零长度的数组是存在于结构体内的，但是不占结构体的size。
//你可以简单的理解为一个没有内容的占位标识，直到我们给结构体分配了内存，这个占位标识才变成了一个有长度的数组。

//gdb 3.Flexible_Array
//(gdb) p thisline
//$1 = (struct line *) 0x601010
// 
//(gdb) p *thisline
//$2 = {length = 10, contents = 0x601010 "\n"}
// 
//(gdb) p thisline->contents
//$3 = 0x601014 "aaaaaaaaaa"
void test1()
{
	struct line {
	   int length;
	   char contents[0]; // C99的玩法是：char contents[]; 没有指定数组长度
	};

    int this_length=10;
    struct line *thisline = (struct line *)
                     malloc (sizeof (struct line) + this_length);
    thisline->length = this_length;
    memset(thisline->contents, 'a', this_length);
	return;//gdb设置断点
}

//看到这里，你会说，为什么要这样搞啊，把contents声明成一个指针，然后为它再分配一下内存不行么？就像下面一样。

//这不一样清楚吗？而且也没什么怪异难懂的东西。
//是的，这也是普遍的编程方式，代码是很清晰，也让人很容易理解。即然这样，那为什么要搞一个零长度的数组？有毛意义？！

//这个事情出来的原因是——我们想给一个结构体内的数据分配一个连续的内存！这样做的意义有两个好处：
//第一个好处是:
//方便内存释放。如果我们的代码是在一个给别人用的函数中，你在里面做了二次内存分配，并把整个结构体返回给用户。
//用户调用free可以释放结构体，但是用户并不知道这个结构体内的成员也需要free，所以你不能指望用户来发现这个事。
//如果我们把结构体的内存以及其成员要的内存一次性分配好了，并返回给用户一个结构体指针，用户做一次free就可以把所有的内存也给释放掉。
//（读到这里，你一定会觉得C++的封闭中的析构函数会让这事容易和干净很多）
//第二个好处是:
//这样有利于访问速度。连续的内存有益于提高访问速度，也有益于减少内存碎片。
//（其实，我个人觉得也没多高了，反正你跑不了要用做偏移量的加法来寻址）
void test2()
{
	struct line {
	   int length;
	   char *contents;
	};
	 
	int this_length=10;
	struct line *thisline = (struct line *)malloc (sizeof (struct line));
	thisline->contents = (char*) malloc( sizeof(char) * this_length );
	thisline->length = this_length;
	memset(thisline->contents, 'a', this_length);
	return;//gdb设置断点
}

//调试程序
//gdb 3.Flexible_Array
//(gdb) b 37
//(gdb) b 66
//(gdb) run
//Breakpoint 1, test1 () at 3.Flexible_Array.c:37
//37		return;//gdb设置断点
//(gdb) x /16b thisline //x:是检查内存的意思 16:是检查的数量 b:是单位是字节
//0x601010:	10	0	0	0	97	97	97	97
//0x601018:	97	97	97	97	97	97	0	0
//(gdb) c
//Continuing.
//Breakpoint 2, test2 () at 3.Flexible_Array.c:66
//66		return;//gdb设置断点
//(gdb) x /16b thisline
//0x601030:	10	0	0	0	0	0	0	0  //length是10, int 4个字节补充3个字节, contents是指针为了内存对齐又补充4个字节
//0x601038:	80	16	96	0	0	0	0	0  //指针是8字节,小端存储,10进制96->16进行60, 10进制16->16进行10, 10进制80->16进行50,结果是0x601050
//(gdb) x /10b thisline->contents
//0x601050:	97	97	97	97	97	97	97	97
//0x601058:	97	97	0	0	0	0	0	0
//(gdb) p thisline->contents
//$6 = 0x601050 "aaaaaaaaaa" //地址:0x601050 内容:"aaaaaaaaaa" 
int main(){
	test1();
	test2();
	return 0;
}
