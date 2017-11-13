#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

/*测试1:注释掉fflush
[root@web11 3.exec]# ./3.exec
BEGIN
total 28
4083029 -rwxr-xr-x  1 root root 6761 Nov 14 00:49 3.exec

[root@web11 3.exec]# ./3.exec  > /tmp/123.txt
[root@web11 3.exec]# cat /tmp/123.txt
total 28
4083029 -rwxr-xr-x  1 root root 6761 Nov 14 00:49 3.exec
*/

/*
测试2:使用fflush
[root@web11 3.exec]# ./3.exec
BEGIN
total 28
4083029 -rwxr-xr-x  1 root root 6877 Nov 14 01:01 3.exec
[root@web11 3.exec]# ./3.exec > /tmp/456
[root@web11 3.exec]# cat /tmp/456
BEGIN(这就是fflush的作用)
total 28
4083029 -rwxr-xr-x  1 root root 6877 Nov 14 01:01 3.exec
*/

int main(int argc, char *argv[])
{
	int ret;

	printf("BEGIN\n");
	//1.exec和fork执行前都要fflush 刷新缓冲区
	fflush(NULL);

	//2.exec 返回值:
	//成功:整个进程就会替换成里面的命令, 下面的所有语句都不会取执行
	//失败:execl返回-1,所以也不用判断了,只要向下执行就是错了
	execl("/bin/ls", "asdada","-li", NULL);
	perror("execl():");
	printf("END\n");//因为如果不出错永远执行不到这句
	return 0;
}
