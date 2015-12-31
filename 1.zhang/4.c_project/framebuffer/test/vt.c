#include <stdio.h>
int main(int argc, const char *argv[])
{
	//vt 控制码实现
	//vt左上点是1，1 而framebuffer左上点是0,0
	//\33 或者\033 VT码的开头标识 
	//[2J是清理屏幕 , [x;yH 光标移动到 x y
	//[4m下划线 ［0m关闭所有属性
	//
	//设定显示字符的属性状态，若有两个以上设定则以分号将代码(n1;n2;...)隔开，
	//除非重新设定 否则原设定属性一直被保留
	printf("\33[2J\33[1;1H\33[31;4mhello\n\033[0m");
	return 0;
}
