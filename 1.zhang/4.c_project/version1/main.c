/*******************************
修改menu.lst
  title XXXXOS (2.6.XX)
  root (hd0,X)
  kernel /boot/vmlinuz-2.6.XXX ro root=XXX quiet vga=0x318

在kernel那行最后添加vga=0x318或vga=792 （前面十六进制，后面十进制，参考下表）

vga=可设置的值可以用工具fbset（没有的话用sudo apt-get install fbset装）,/etc/fb.modes,或sudo hwinfo --framebuffer参考，主要还是以hwinfo为主（sudo apt-get install hwinfo）

下表列出一些常用值：

# FRAMEBUFFER RESOLUTION SETTINGS
# +----------------------------------------------------------------+
# |    640x480    800x600   1024x768 1280x1024 1280x800   1600x1200
# ----+-------------------------------------------------------------
# 256 | 0x301=769 0x303=771 0x305=773 0x307=775            0x31C=796
# 32K | 0x310=784 0x313=787 0x316=790 0x319=793 0x360＝864 0x31D=797
# 64K | 0x311=785 0x314=788 0x317=791 0x31A=794 0x361＝865 0x31E=798
# 16M | 0x312=786 0x315=789 0x318=792 0x31B=795 0x362＝866 0x31F=799
# +----------------------------------------------------------------+
*******************************/
#include <stdio.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

#include "fb_draw.h"
#include "mydraw.h"

static int pos_x=XNUM/2, pos_y=0;
static char arr[YNUM][XNUM] = {};
static int cur_type = 0;


void sig_handler(int s)
{
	alarm(1);
	if(candown(pos_x, pos_y, cur_type, arr)) {
		draw_type(pos_x, pos_y, cur_type, BACK);
		pos_y++;
		draw_type(pos_x, pos_y, cur_type, COLOR);
		draw_grid();
	}else {
		draw_flag(pos_x, pos_y, cur_type, arr);
		if(gameover(arr)) {
			exit(0);
		}
		deleteone(arr);
		pos_x = XNUM/2; pos_y = 0;
		cur_type = rand()%14;
	}
}


static void key_init(void)
{
	struct termios tc;

	tcgetattr(0, &tc);
	tc.c_lflag &= (~ICANON);
	tc.c_lflag &= (~ECHO);
	tcsetattr(0, TCSANOW, &tc);
}

int main(int argc, const char *argv[])
{
	int ret;
	char ch;
	
	srand(time(NULL));
	ret = fb_open();
	key_init();	
	draw_back();
	draw_grid();
	signal(SIGALRM, sig_handler);
	alarm(1);

	while (1) {
		ch = getchar();
		switch (ch) {
			case 'a':
				if(canleft(pos_x, pos_y, cur_type, arr)) {
					draw_type(pos_x, pos_y, cur_type, BACK);
					pos_x--;
				}
				break;
			case 'd':
				if(canright(pos_x, pos_y, cur_type, arr)) {
					draw_type(pos_x, pos_y, cur_type, BACK);
					pos_x++;
				}
				break;	
			case 's':
				if(candown(pos_x, pos_y, cur_type, arr)) {
					draw_type(pos_x, pos_y, cur_type, BACK);
					pos_y++;
				}
				break;
			case 'w':
				if(canchange(pos_x,pos_y,cur_type,arr)) {
					draw_type(pos_x, pos_y, cur_type, BACK);
					type_chg(&cur_type);
				}
				break;
			default :
				break;
		}
		draw_type(pos_x, pos_y, cur_type, COLOR);
		draw_grid();
		if(!candown(pos_x, pos_y, cur_type, arr)) {
			draw_flag(pos_x, pos_y, cur_type, arr);
			if(gameover(arr)) {
				exit(0);
			}
			deleteone(arr);
			pos_x = XNUM/2; pos_y = 0;
			cur_type = rand()%14;
		}
	}

	fb_close();

	return 0;
}

