#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#include "fb_draw.h"

#define PINK 0xfa
#define BLUE 0x1a

#define COL_CANVAS 4
#define ROW_CANVAS 4

#define ROW 10
#define COL 20
#define DEGREE 2

#define DRAW_BLOCK_SIZE 20

enum PIC_TYPE{BACK_GROUP,GUN,RECT,FUCK,LIANG,ZUO,YOU};
enum POSITION{UP,LEFT,DOWN,RIGHT};

typedef struct{
	enum PIC_TYPE type;
	enum POSITION pos;
	char (*arr)[COL_CANVAS];
}block,*block_entry_pointer;

static int max_x, max_y, min_x, min_y;
static int current_max_x, current_max_y, curret_min_x, current_min_y;

static int (*russian_p)[COL];
static int autodown_y;
static int autodown_x;
static block_entry_pointer block_entry_p = NULL;

static void draw_element(int x, int y, int color);
void sig_handler(int s);
void sig_handler_back(int s);
void draw_picture(int row_canvas, int color);
void init(int (**russian_p)[COL], int row);

void reset_block()
{
	int i,j;
	int x,y;
	for (i = 0; i < ROW_CANVAS; i++) {
		for (j = 0; j < COL_CANVAS; j++) {
			if ( block_entry_p->arr[i][j] == 1 ) {
				block_entry_p->arr[i][j] = 0;
				x = i * DRAW_BLOCK_SIZE;
				y = j * DRAW_BLOCK_SIZE;
				draw_element(x+autodown_x, y+autodown_y, PINK);
			}
		}
	}
}

void change()
{
	reset_block();
	block_entry_p->pos = (block_entry_p->pos+1)%4;
	draw_picture(ROW_CANVAS, BLUE);
}

void init_current_block()
{
	block_entry_p = calloc(1,sizeof(block));//4x16=56
	block_entry_p->arr = calloc(1,sizeof(char) * COL_CANVAS *16);//4x16=56
	block_entry_p->type = FUCK;
	block_entry_p->pos = UP;
}

void left()
{
	if (autodown_x-10 < min_x) {
		return;
	}
	draw_picture(ROW_CANVAS, 0xfa);
	autodown_x-=10;
	printf("autodown_x %d\n", autodown_x);
	draw_picture(ROW_CANVAS, 0x1a);
}

void right()
{
	if (autodown_x+10 > max_x) {
		return;
	}
	draw_picture(ROW_CANVAS, 0xfa);
	autodown_x+=10;
	draw_picture(ROW_CANVAS, 0x1a);
}

void down()
{
	if (autodown_y+10 > max_y) {
		return;
	}
	draw_picture(ROW_CANVAS, 0xfa);
	autodown_y+=10;
	draw_picture(ROW_CANVAS, 0x1a);
}

int main(int argc, const char *argv[])
{
	int ret,ch;
	ret = fb_open();	

	struct termios tc, old_tc;
	tcgetattr(0, &tc);//0是输入
	tcgetattr(0, &old_tc);//用于下面恢复
	tc.c_lflag &= (~ICANON);//启动食品模式
	tc.c_lflag &= (~ECHO);//ECHO 使用回显  &~(ECHO) 取消回显
	tcsetattr(0, TCSANOW, &tc);//TCSANOW 立刻生效

	init(&russian_p, ROW);

	init_current_block();
	draw_picture(ROW_CANVAS, 0x1a);

	signal(SIGALRM, sig_handler);
	alarm(1);

	while (1) {
		ch = getchar();
		if (ch == 'q') {//exit
			break;
		}
		if (ch =='w') {
			change();
		}
		if (ch =='a') {
			left();
		}
		if (ch =='s') {
			down();
 		}
		if (ch =='d') {
			right();
		}
	}

	tcsetattr(0, TCSANOW, &old_tc);
	fb_close();

	return 0;
}

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
	draw_picture(ROW_CANVAS, 0xfa);
	autodown_y+=10;
	draw_picture(ROW_CANVAS, 0x1a);
	alarm(1);	
}

void draw_picture(int row_canvas, int color){
	int i,j;
	int x,y;

	switch(block_entry_p->type){
		case FUCK:
			switch(block_entry_p->pos){
				case UP:
					block_entry_p->arr[0][1] = 1;
					block_entry_p->arr[1][0] = 1;//left
					block_entry_p->arr[1][1] = 1;//down
					block_entry_p->arr[1][2] = 1;//right
				break;
				case DOWN:
					block_entry_p->arr[1][1] = 1;//left
					block_entry_p->arr[0][0] = 1;
					block_entry_p->arr[0][1] = 1;//
					block_entry_p->arr[0][2] = 1;//
				break;
				case LEFT:
					block_entry_p->arr[0][0] = 1;
					block_entry_p->arr[1][0] = 1;
					block_entry_p->arr[1][1] = 1;
					block_entry_p->arr[2][0] = 1;
				break;
				case RIGHT:
					block_entry_p->arr[0][1] = 1;
					block_entry_p->arr[1][0] = 1;
					block_entry_p->arr[1][1] = 1;
					block_entry_p->arr[2][1] = 1;
				break;
				default:
				break;
			}
		break;
		default:break;
	}

	for (i = 0; i < row_canvas; i++) {
		for (j = 0; j < COL_CANVAS; j++) {
			if ( block_entry_p->arr[i][j] == 1 ) {
				x = i*DRAW_BLOCK_SIZE;
				y = j*DRAW_BLOCK_SIZE;
				draw_element(x+autodown_x, y+autodown_y, color);
			}
		}
	}

}

void init(int (**russian_p)[COL],int row)
{
	int i,j;
	int x,y;
	int is_init = 0;

	*russian_p = malloc(sizeof(int) * COL * row);
	memset(*russian_p, 0, sizeof(int) * COL * row);

	for (i = 0; i <= row; i++) {
		for (j = 0; j <=COL; j++) {
			x=i*DRAW_BLOCK_SIZE;
			y=j*DRAW_BLOCK_SIZE;
			if (is_init == 0) {
				max_x = min_x = x;
				max_y = min_y = y;
				is_init = 1;
			}else{
				if (x > max_x) {
					max_x = x;
				}
				if (x < min_x) {
					min_x = x;
				}
				if (y > max_y) {
					max_y = y;
				}
				if (y < min_y) {
					min_y = y;
				}
			}
			draw_element(x, y, 0xfa);
		}
	}

}

