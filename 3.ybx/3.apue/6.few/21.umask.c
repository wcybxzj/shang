#include <stdio.h>
#include <stdlib.h>
int main(int argc, const char *argv[])
{
	umask(0);//设置当前umask
	system("rm 123.txt");
	system("touch 123.txt");//文件默认权限666 & ~umask
	return 0;
}
