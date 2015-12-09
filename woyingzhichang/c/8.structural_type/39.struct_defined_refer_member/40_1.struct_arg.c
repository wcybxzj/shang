#include <stdio.h>

struct simp_st{
	int i;
	char ch;
	float f;
};

//s1传递结构体在函数中需要开辟一个同样大小的空间复制传递过来的内容,开销太大
void func_struct(struct simp_st s1)
{
	printf("%d\n", sizeof(s1));//12
}

void func_pointer(struct simp_st *p)
{
	printf("%d\n", sizeof(p));//32位机器 4
}

int main(int argc, const char *argv[])
{
	struct simp_st a;
	struct simp_st *p = &a;
	func_struct(a);// func(a.i, a.ch, a.f)

	func_pointer(p);


	return 0;
}
