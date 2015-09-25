#include <stdio.h>
#include <termios.h>
#include <unistd.h>


int main(void)
{
	int ch;

	tcgetattr(0, &tc);//0是输入
	tcgetattr(0, &old_tc);//用于下面恢复
	tc.c_lflag &= (~ICANON);//启动食品模式
	tc.c_lflag &= (~ECHO);//ECHO 使用回显  &~(ECHO) 取消回显
	tcsetattr(0, TCSANOW, &tc);//TCSANOW 立刻生效

	while (1) {
		ch = getchar();
		if (ch == 'a') {
			printf("this is a test\n");
		}
		printf("%c %d\n",ch, ch);

		if (ch =='q') {
			break;
		}
	}

	tcsetattr(0, TCSANOW, &old_tc);
	return 0;
}
