#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#include "fb_draw.h"

#define COL_CANVAS 4
#define ROW_CANVAS 4

#define ROW 10
#define COL 20

#define DRAW_BLOCK_SIZE 20

typedef struct{
	char (*arr)[COL_CANVAS];
	int current_pos;
}block,*block_entry_pointer;

enum PIC_TYPE{BACK_GROUP,GUN,RECT,FUCK,LIANG,ZUO,YOU};
enum POSITION{UP,DOWN,LEFT,RIGHT};

static enum PIC_TYPE block_status;
static int (*russian_p)[COL];
//static char (*block_p)[COL_CANVAS];
static int autodown_x;
static block_entry_pointer block_entry_p;

//function
static void draw_element(int x, int y, int color);
void sig_handler(int s);
//void draw_picture(enum PIC_TYPE type, char (**block_p)[COL_CANVAS],int row_canvas,int color);
void draw_picture(enum PIC_TYPE type, ,int row_canvas,int color);
void init(int (**russian_p)[COL], int row);
//void change(char (**block_p)[COL_CANVAS]);

int main(int argc, const char *argv[])
{
	int ret,ch;
	int i, j;

	ret = fb_open();	

	//if error

	signal(SIGALRM, sig_handler);
	alarm(1);

	struct termios tc, old_tc;
	tcgetattr(0, &tc);//0是输入
	tcgetattr(0, &old_tc);//用于下面恢复
	tc.c_lflag &= (~ICANON);//启动食品模式
	tc.c_lflag &= (~ECHO);//ECHO 使用回显  &~(ECHO) 取消回显
	tcsetattr(0, TCSANOW, &tc);//TCSANOW 立刻生效

	while (1) {
		ch = getchar();
		//printf("%c %d\n",ch, ch);
	void	if (ch == 'q') {//exit
			break;
		}
		if (ch =='w') {//pause
			//change(&block_p);
		}
		if (ch =='a') {//pause
		}
		if (ch =='s') {//pause

		}
		if (ch =='d') {//pause

		}
	}

	tcsetattr(0, TCSANOW, &old_tc);
	fb_close();

	return 0;
}

//void change(char (**block_p)[COL_CANVAS])
//{
//	return;
//}


static void draw_element(int x, int y, int color)
{
	int i, j;	
	for (i = 0; i < DRAW_BLOCK_SIZE; i++) {
		for (j = 0; j < DRAW_BLOCK_SIZE; j++) {
			fb_draw_point(x+j, y+i, color);
		}
	}
}

void sig_handler(int s)
{
	alarm(1);	
	init(&russian_p, ROW);
	draw_picture(FUCK, &block_p, ROW_CANVAS, 0xfa);
	autodown_x+=10;
	draw_picture(FUCK, &block_p, ROW_CANVAS, 0x1a);
}

void draw_picture(enum PIC_TYPE type, char (**block_p)[COL_CANVAS],int row_canvas, int color){
	int i,j;
	int x,y;
	*block_p = calloc(1,sizeof(char) * COL_CANVAS * row_canvas);//4x16=56


	//0,1 1,0 1,1 1,2
	if (type == FUCK) {
		(*block_p)[0][1] = 1;
		(*block_p)[1][0] = 1;
		(*block_p)[1][1] = 1;
		(*block_p)[1][2] = 1;
	}

	for (i = 0; i < row_canvas; i++) {
		for (j = 0; j < COL_CANVAS; j++) {
			if ( (*block_p)[i][j] == 1 ) {
				//printf("i %d j %d",i ,j);
				//printf(" =%d |",(*block_p)[i][j]);

				x = i*DRAW_BLOCK_SIZE;
				y = j*DRAW_BLOCK_SIZE;

				//printf("x %d y %d||||||",x ,y);
				//draw_element(y, x+autodown_x , 0x1a);
				draw_element(x+autodown_x, y, color);
			}
		}
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

void init(int (**russian_p)[COL], int row)
{
	int i,j;
	int x,y;

	*russian_p = malloc(sizeof(int) * COL * row);
	memset(*russian_p,0,sizeof(int) * COL * row);

	printf("\33[2J");
	for (i = 0; i <= row; i++) {
		for (j = 0; j <=COL; j++) {
			x=i*DRAW_BLOCK_SIZE;
			y=j*DRAW_BLOCK_SIZE;
			draw_element(x, y, 0xfa);
		}
	}
	//draw(BACK_GROUP);
	//1.clean window
	//2.background
}

