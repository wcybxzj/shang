#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

typedef enum { FALSE, TRUE } Boolean;

static volatile sig_atomic_t got_sig_io = 0;


void errExit(char *str)
{
	printf("%s\n", str);
	exit(1);
}


//例子:/shangguan/1.zhang/4.c_project/framebuffer/test/keyboard.c
int ttySetCbreak(int fd, struct termios *prevTermios)
{
	struct termios t;

	if (tcgetattr(fd, &t) == -1)
		return -1;

	if (prevTermios != NULL)
		*prevTermios = t;

	t.c_lflag &= ~ICANON;//以字为单位进行获取，而不是行位单位
	t.c_lflag &= ~ECHO;//不回显输入的内容

	//t.c_lflag |= ISIG;
	//t.c_iflag &= ~ICRNL;

	//t.c_cc[VMIN] = 1;                   /* Character-at-a-time input */
	//t.c_cc[VTIME] = 0;                  /* with blocking */

	if (tcsetattr(fd, TCSAFLUSH, &t) == -1)
		return -1;

	return 0;
}

static void sigio_handler(int sig)
{
	got_sig_io = 1;
}

//信号驱动IO
int main(int argc, const char *argv[])
{

	Boolean done;
	int i, ret, flags, cnt /*,ch*/;
	//int ch;//not ok,下面的 ch=='#'判断不出来
	int ch=0;//ok
	//char ch;//ok
	struct termios origTermios;
	struct sigaction sa;

	//监听信号驱动IO事件
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = sigio_handler;
	if (sigaction(SIGIO, &sa, NULL)== -1) {
		exit(1);
	}

	//给fd设置属主为当前进程
	ret = fcntl(STDIN_FILENO, F_SETOWN, getpid());
	if (ret==-1) {
		printf("fcntl\n");
		exit(1);
	}

	//fd 设置为非阻塞并且异步
	flags = fcntl(STDIN_FILENO, F_GETFL);
	ret = fcntl(STDIN_FILENO, F_SETFL, flags|O_ASYNC|O_NONBLOCK);
	if (ret ==-1) {
		printf("fcntl\n");
		exit(1);
	}

	//cbreak模式
	if (ttySetCbreak(STDIN_FILENO, &origTermios) == -1)
	{
		printf("error\n");
		exit(1);
	}

	//获取输入
	cnt = 0;
	done = FALSE;
	while (!done) {
		for (i = 0; i < 100000000; i++) {//在读取前短暂的卡住程序
			continue;
		}
		//终端可以读取
		if (got_sig_io) {
			got_sig_io = 0;
			while( (ret = read(STDIN_FILENO, &ch, 1)>0) && !done ) {
				printf("cnt:%d, read:%c, done:%d\n",cnt, ch, done);
				if (ch=='#') {
					done = TRUE;
				}
			}
		}
		cnt++;
	}

	//恢复终端状态
	ret = tcsetattr(STDIN_FILENO, TCSAFLUSH, &origTermios);
	if (ret == -1) {
		printf("tcsetattr\n");
		exit(1);
	}
	exit(0);
}
