#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

/*
//在版本1中的问题:   
gcc -S 17.5sec_sig_volatile.c -O1 -o 17.5sec_sig_volatile_O1.s
gcc -S 17.5sec_sig_volatile.c -o 17.5sec_sig_volatile.s
volatile 优化loop避免优化中的问题
*/

//版本1:最普通不能再O1优化中使用
// O1优化后在while(loop)中loop没是每次去取值造成一直死循环
static int loop=1;

//版本2:volatile 可以在O1中使用
//表示一定要到变量定义的位置取变量的值，而不要轻信从内存拿值
//static volatile int loop=1;

//版本3:增加信号原子性
//static volatile sig_atomic_t loop=1;
void alarm_handler(int s){
	loop=0;
}

int main(int argc, const char *argv[])
{
	int64_t a=1;
	signal(SIGALRM, alarm_handler);
	alarm(5);
	while (loop) {
		a++;
	}
	printf("%lld\n", a);
	exit(0);
}
