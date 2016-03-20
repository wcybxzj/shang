#include <stdio.h>
#include <unistd.h>
int main(int argc, const char *argv[])
{
	alarm(20);
	alarm(1);
	alarm(3);//最后一个时钟会覆盖之前的时钟
	while (1);
	printf("SIGALRM 默认动作是杀死当前进程,属于异常终止进程,此句不会执行\n");
	return 0;
}
