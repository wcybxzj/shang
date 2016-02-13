#include <stdio.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
	alarm(100);

	//while (1);//盲等,让一个cpu核跑满100%,htop或者top加1 进行查看
	getchar();
	//pause();//人为制造一个阻塞的系统调用,专用等待信号来进行打断,最重要的是不是盲等

	return 0;
}
