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

#include "fb_draw.h"

#define BACK_HIGH 700
#define BACK_WIDTH 600

static int pos_x=100, pos_y = 100;
enum PIC_TYPE{BACK_GROUP,GUN,RECT,FUCK,LIANG,ZUO,YOU};

static void draw_element(int x, int y, int color);
void sig_handler(int s);
void draw(enum PIC_TYPE type);
void init();

int main(int argc, const char *argv[])
{
	init();
	int ret;
	int i, j;

	ret = fb_open();	
	//if error
	//draw_element(pos_x, pos_y, 0xfa);

	//signal(SIGALRM, sig_handler);
	//alarm(1);
	//while (1);

	fb_close();

	return 0;
}

static void draw_element(int x, int y, int color)
{
	int i, j;	
	for (i = 0; i < 50; i++) {
		for (j = 0; j < 50; j++) {
			fb_draw_point(x+j, y+i, color);
		}
	}
}

void sig_handler(int s)
{
	alarm(1);	

	draw_element(pos_x, pos_y, 0x00);//0x00 黑 0xff白
	pos_y+=10;
	draw_element(pos_x, pos_y, 0xfa);
}

void draw(enum PIC_TYPE type){
	if (type == BACK_GROUP) {
		
		draw_element(BACK_WIDTH, BACK_HIGH, 0xfa);
	}

		//case GUN:
		//	break;
		//case RECT:
		//	break;
		//case FUCK:
		//	break;
		//case LIANG:
		//	break;
		//case ZUO:
		//	break;
		//case YOU:
		//	break;
}

void init()
{
	draw(BACK_GROUP);
	//1.clean window
	//2.background
}

