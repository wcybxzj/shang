#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

//先创建文件
//dd if=/dev/zero of=/tmp/out bs=2000M count=1	
int main(int argc, const char *argv[])
{
	FILE * fp;
	fp = fopen("/tmp/out","r");
	if (NULL == fp) {
		perror("fopen():");
		exit(1);
	}

	if (unlink("/tmp/out")<0) {
		perror("unlink():");
		exit(1);
	}
	//unlink后因为硬链接为0
	//本应该删除目录项并且删除物理数据块
	//但是进程正在占用资源,所以文件在目录项中看不到了,但是物理硬盘还占用着

	printf("进程开始\n");
	sleep(30);//临时文件的生命周期

	//执行df 查看硬盘占用情况 88%

	fclose(fp);
	printf("进程执行结束\n");

	//执行df 查看硬盘占用情况 82%

	return 0;
}
