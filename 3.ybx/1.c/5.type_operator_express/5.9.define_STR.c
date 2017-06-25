#include <stdio.h>
#define STR(s) #s

/*
c.biancheng.net
"c.biancheng.net"
*/
void func1()
{
    printf("%s\n", STR(c.biancheng.net));
    printf("%s\n", STR("c.biancheng.net"));
}

int main() {
	func1();
    return 0;
}
