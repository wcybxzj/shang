#include "tlpi_hdr.h"
/*
[root@web11 34.pgsjc]# echo $$
11323
[root@web11 34.pgsjc]# ./1.front_processgroup 
pid:11417 pgid:11417
当前前段进程组:11417
*/
int main(int argc, const char *argv[])
{
	printf("pid:%d pgid:%d\n",getpid(), getpgid(0));
	printf("当前前段进程组:%d\n",tcgetpgrp(STDIN_FILENO));

	return 0;
}
