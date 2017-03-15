#include <stdio.h>
struct test{
    int i;
    short c;
    char *p;
};

//当初始化pt后,就算是pt为NULL，访问其中的成员时，其实就是在访问相对于pt的内址
 
//(gdb) b 20
//(gdb) run
//(gdb)  p pt
//$1 = (struct test *) 0x0
//(gdb) p pt->i
//Cannot access memory at address 0x0
//(gdb) p pt->c
//Cannot access memory at address 0x4
//(gdb) p pt->p
//Cannot access memory at address 0x8
int main(){
    struct test *pt=NULL;
	printf("%d\n", sizeof(struct test));//16
    return 0;
}
