#include <stdio.h>


//第2种extern "C"写法
//extern "C" {
#include "moduleA.h"
//}
int main(int argc, const char *argv[])
{
	foo(11,22);
	return 0;
}
