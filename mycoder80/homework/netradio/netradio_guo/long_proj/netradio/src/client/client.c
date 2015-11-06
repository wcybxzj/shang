#include <stdio.h>
#include <stdlib.h>

#include "client.h"

//#include "../include/proto.h"
#include <proto.h>

/*
 *	-M		指定多播组
 *	-P		指定收接端口
 *	-p		指定播放器
 *	-H		显示帮助
 * */

struct client_conf_st client_conf={\
			.mgroup = DEFAULT_MGROUP,\
			.rcvport = DEFAULT_RCVPORT,\
			.player = DEFAULT_PLAYER};

int main(int argc,char **argv)
{
	
	/*conf处理 getopt*/

	/*socket init*/

	/*pipe*/

	/*fork*/

	/*父进程接收网络上的包，交给子进程去播放*/

	exit(0);
}





