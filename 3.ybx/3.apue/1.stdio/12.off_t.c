#include <stdio.h>

//gcc  -D_FILE_OFFSET_BITS=64 12.off_t.c
//./a.out 
//64
//8

//gcc  -D_FILE_OFFSET_BITS=32 12.off_t.c
//./a.out 
//32
//4

//或者直接定义
//#define _FILE_OFFSET_BITS 64
int main(int argc, const char *argv[])
{
	printf("%d\n", _FILE_OFFSET_BITS);
	printf("%d\n", sizeof(off_t));
	return 0;
}
