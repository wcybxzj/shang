#include <stdio.h>
#include <proto.h>
#include <getopt.h>
#include "server.h"

/*
 *  -M      指定多播组
 *  -P      指定收接端口
 *  -C		指定测试频道
 *  -F		前台调试运行
 *  -D		媒体库位置
 *  -e		设置网卡
 *  -H      显示帮助
 * */

int main()
{
	struct mlib_chn_st *listptr;
	int listsize;

	/*命令行分析*/

	/*守护进程*/

	/*socket init*/

	/*获取频道列表信息（从medialib中）*/
	err = mlib_getchnlist(&listptr,&listsize);		
	if(err)
	{
		syslog();
	}

	/*创建thr_list*/
	err = thr_list_create();
	if()
	{

	}

	/*创建thr_channel*/
	/*1:200  100:200  4:200  200:200*/

	for(i = 0 ; i < listsize; i++)
		thr_channel_create();


	while(1)
		pause();

	exit(0);
}


