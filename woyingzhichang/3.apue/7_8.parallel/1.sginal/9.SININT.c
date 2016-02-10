#include <stdio.h>
#include <signal.h>
#define SIZE 10
//信号从收到到响应是有一个延迟的
//证明方式运行程序立刻发送SIGINT，看C^!后是还会打印字符才结束程序
int main(int argc, const char *argv[])
{
	while (1) {
		char buf[SIZE] ="a";
		write(1, buf, SIZE);
	}
	return 0;
}

