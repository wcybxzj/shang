#include <stdio.h>

/*
 *  -M      指定多播组
 *  -P      指定接收端口
 *  -p      指定播放器
 *  -C		指定测试频道号
 *  -F		前台调试运行
 *  -H      显示帮助
 * */


int main()
{
	struct mlib_chn_st *listptr;
	int listsize;
	
	/*conf处理*/	

	/*socket初始化*/	

	/*获取频道列表信息*/	
	if(mlib_getchnlist(&listptr,&listsize) != 0)
	{
		syslog();
	}

	/*创建thr_list*/	
	thr_list_create(listptr,listsize);		
	/*if error*/


	/*创建thr_channel  n : n */	
	for(i = 0 ; i < listsize ; i++)	
		thr_channel_create();


	while(1)
		pause();


	exit(0);
}





