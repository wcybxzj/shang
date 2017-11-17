#include <stdio.h>
#include <unistd.h>

//起因,我写1.demo_sigio.c 在判断read的时候总是有问题
//发现ch的4个位置影响下面的ch=='#'是否成功
int main(int argc, const char *argv[])
{
	int asdas, aaa/*, ch*/;//fail
	//int ch;//fail
	//int ch=0;//ok
	char ch;//ok
	read(STDIN_FILENO, &ch,1);

	if (ch=='#') {
		printf("##############\n");
	}

	return 0;
}
