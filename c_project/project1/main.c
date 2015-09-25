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

static int pos_x=16, pos_y=0;
static char arr[24][32] = {};
static int cur_shp = 0, lst_shp = 0;


void leftmovea(void)
{
	if((pos_x-1) > 0 && !arr[pos_y][pos_x-1] && !arr[pos_y+1][pos_x-2]) {
		reset_arra(pos_x, pos_y, arr);
		pos_x--;
		set_arra(pos_x, pos_y, arr);
	}
	draw_fresh(arr);
}

void rightmovea(void)
{
	if((pos_x+1) < 31 && !arr[pos_y][pos_x+1] && !arr[pos_y+1][pos_x+2]) {
		reset_arra(pos_x, pos_y, arr);
		pos_x++;
		set_arra(pos_x, pos_y, arr);
	}
	draw_fresh(arr);
}

void downmovea(void)
{
	int i, j;

	if((pos_y+1) < 23 && !arr[pos_y+2][pos_x]\
		&& !arr[pos_y+2][pos_x-1] && !arr[pos_y+2][pos_x+1]) {
		reset_arra(pos_x, pos_y, arr);
		pos_y++;
		set_arra(pos_x, pos_y, arr);
	}else {
		pos_x = 16;
		pos_y = 0;
		cur_shp = rand() % 3;
	}
	draw_fresh(arr);
	if(is_oneline(arr)) {
		for(i = 0; i < 32; i++) {
			for(j = 0; j <23; j++) {
				arr[j+1][i] = arr[j][i];
			}
		}
		draw_fresh(arr);
	}
}

void leftmoveb(void)
{
	if(pos_x > 0 && !arr[pos_y][pos_x-1] && !arr[pos_y+1][pos_x-1]\
			&& !arr[pos_y+2][pos_x-1]) {
		reset_arrb(pos_x, pos_y, arr);
		pos_x--;
		set_arrb(pos_x, pos_y, arr);
	}
	draw_fresh(arr);
}

void rightmoveb(void)
{
	if((pos_x+1) < 31 && !arr[pos_y][pos_x+1] &&\
		   	!arr[pos_y+1][pos_x+2] && !arr[pos_y+2][pos_x+1]) {
		reset_arrb(pos_x, pos_y, arr);
		pos_x++;
		set_arrb(pos_x, pos_y, arr);
	}
	draw_fresh(arr);
}

void downmoveb(void)
{
	int i, j;

	if((pos_y+2) < 23 && !arr[pos_y+3][pos_x] && !arr[pos_y+2][pos_x+1]) {
		reset_arrb(pos_x, pos_y, arr);
		pos_y++;
		set_arrb(pos_x, pos_y, arr);
	}else {
		pos_x = 16;
		pos_y = 0;
		cur_shp = rand() % 3;
	}
	draw_fresh(arr);
	if(is_oneline(arr)) {
		for(i = 0; i < 32; i++) {
			for(j = 0; j <23; j++) {
				arr[j+1][i] = arr[j][i];
			}
		}
		draw_fresh(arr);
	}
}
void leftmovec(void)
{
	if((pos_x-1) > 0 && !arr[pos_y][pos_x-1] && \
			!arr[pos_y+1][pos_x-2] && !arr[pos_y+2][pos_x-1]) {
		reset_arrc(pos_x, pos_y, arr);
		pos_x--;
		set_arrc(pos_x, pos_y, arr);
	}
	draw_fresh(arr);
}

void rightmovec(void)
{
	if(pos_x < 31 && !arr[pos_y][pos_x+1] && \
			!arr[pos_y+1][pos_x+1] && !arr[pos_y+2][pos_x+1]) {
		reset_arrc(pos_x, pos_y, arr);
		pos_x++;
		set_arrc(pos_x, pos_y, arr);
	}
	draw_fresh(arr);
}

void downmovec(void)
{
	int i, j;

	if((pos_y+2) < 23 && !arr[pos_y+3][pos_x] && !arr[pos_y+2][pos_x-1]) {
		reset_arrc(pos_x, pos_y, arr);
		pos_y++;
		set_arrc(pos_x, pos_y, arr);
	}else {
		pos_x = 16;
		pos_y = 0;
		cur_shp = rand() % 3;
	}
	draw_fresh(arr);
	if(is_oneline(arr)) {
		for(i = 0; i < 32; i++) {
			for(j = 0; j <23; j++) {
				arr[j+1][i] = arr[j][i];
			}
		}
		draw_fresh(arr);
	}
}

void sig_handler(int s)
{
	alarm(1);	
	switch(cur_shp) {
		case 0:downmovea();break;
		case 1:downmoveb();break;
		case 2:downmovec();break;
		default:break;
	}
}

void change(void)
{
	cur_shp++;
	if(cur_shp == 3)
		cur_shp = 0;
	switch(cur_shp) {
		case 0:
			switch(lst_shp) {
				case 0:reset_arra(pos_x,pos_y,arr);break;
				case 1:reset_arrb(pos_x,pos_y,arr);break;
				case 2:reset_arrc(pos_x,pos_y,arr);break;
				default:break;
			}
			set_arra(pos_x,pos_y,arr);break;
		case 1:
			switch(lst_shp) {
				case 0:reset_arra(pos_x,pos_y,arr);break;
				case 1:reset_arrb(pos_x,pos_y,arr);break;
				case 2:reset_arrc(pos_x,pos_y,arr);break;
				default:break;
			}
			set_arrb(pos_x,pos_y,arr);break;
		case 2:
			switch(lst_shp) {
				case 0:reset_arra(pos_x,pos_y,arr);break;
				case 1:reset_arrb(pos_x,pos_y,arr);break;
				case 2:reset_arrc(pos_x,pos_y,arr);break;
				default:break;
			}
			set_arrc(pos_x,pos_y,arr);break;
		default:break;
	}
	lst_shp = cur_shp;
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
	signal(SIGALRM, sig_handler);
	alarm(1);

	while (1) {	
		ch = getchar();
		switch (ch) {
			case 'a': 
				switch(cur_shp) {
					case 0:leftmovea();break;
					case 1:leftmoveb();break;
					case 2:leftmovec();break;
					default:break;
				}
				break;
			case 'd': 
				switch(cur_shp) {
					case 0:rightmovea();break;
					case 1:rightmoveb();break;
					case 2:rightmovec();break;
					default:break;
				}
				break;	
			case 's':
				switch(cur_shp) {
					case 0:downmovea();break;
					case 1:downmoveb();break;
					case 2:downmovec();break;
					default:break;
				}
				break;	
			case 'w':change();break;
			default :break;
		}
	}

	fb_close();

	return 0;
}

