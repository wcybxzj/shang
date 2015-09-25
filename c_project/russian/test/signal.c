#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

void alarm_handler(int s)
{
	alarm(1);
	printf("方块落下\n");
}

int main(int argc, const char *argv[])
{
	int ch;
	signal(SIGALRM, alarm_handler);
	alarm(1);

	struct termios tc, old_tc;
	tcgetattr(0, &tc);//0是输入
	tcgetattr(0, &old_tc);//用于下面恢复
	tc.c_lflag &= (~ICANON);//启动食品模式
	tc.c_lflag &= (~ECHO);//ECHO 使用回显  &~(ECHO) 取消回显
	tcsetattr(0, TCSANOW, &tc);//TCSANOW 立刻生效

	while (1) {
		ch = getchar();
		printf("%c %d\n",ch, ch);
		if (ch =='q') {//exit
			break;
		}
		if (ch =='p') {//pause
			break;
		}
	}

	tcsetattr(0, TCSANOW, &old_tc);
	return 0;
}
