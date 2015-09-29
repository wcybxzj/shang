#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#include "fb_draw.h"

#define PINK 0xfa
#define BLUE 0x1a
#define YELLOW 0x2a

#define COL_CANVAS 4
#define ROW_CANVAS 4

#define ROW 10
#define COL 20
#define DEGREE 2

#define DRAW_BLOCK_SIZE 20

enum PIC_TYPE{BACK_GROUP,GUN,RECT,FUCK,LIANG,ZUO,YOU};
enum POSITION{UP,RIGHT,DOWN,LEFT};
enum IS_DRAW{IS_DRAW, NOT_DRAW};

typedef struct{
	enum PIC_TYPE type;
	enum POSITION pos;
	char (*arr)[COL_CANVAS];
}block,*block_entry_pointer;

static int max_x, max_y, min_x, min_y;
static int current_max_x, current_max_y, current_min_x;
static int (*russian_p)[COL];
static int autodown_y;
static int autodown_x;
static block_entry_pointer block_entry_p = NULL;
static char tmp_arr[ROW][COL]={};

static void draw_element(int x, int y, int color);
void sig_handler(int s);
void sig_handler_back(int s);
void draw_picture(int row_canvas, int color, int is_draw);
void init();
void verify(enum PIC_TYPE type, enum POSITION pos, int row_canvas,
	   	int *test_current_max_x, int *test_current_min_x, int *test_current_max_y);
void copy();


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
	int is_allow = 1;
	int test_current_max_x, test_current_min_x, test_current_max_y;
	enum POSITION next_pos = (block_entry_p->pos+1)%4;

	verify(block_entry_p->type, next_pos, ROW_CANVAS,
					&test_current_max_x, &test_current_min_x, &test_current_max_y);

	if (block_entry_p->type == FUCK) {
		if (next_pos == DOWN) {
			if (test_current_min_x + autodown_x < min_x) {
				is_allow = 0;
			}
		}else if(next_pos == UP){
			if (test_current_max_x + autodown_x > max_x) {
				is_allow = 0;
			}
		}else if(next_pos == RIGHT){
			if (test_current_max_y + autodown_y > max_y) {
				is_allow = 0;
			}
		}
	}

	//printf("is_allow %d", is_allow);
	if(is_allow == 1){
		reset_block();
		block_entry_p->pos = next_pos;
		draw_picture(ROW_CANVAS, BLUE, IS_DRAW);
	}
}

void init_current_block()
{
	block_entry_p = calloc(1, sizeof(block));
	block_entry_p->arr = calloc(COL_CANVAS * ROW_CANVAS, sizeof(char));
	block_entry_p->type = FUCK;
	block_entry_p->pos = UP;
}

void left()
{
	if (current_min_x+autodown_x-DRAW_BLOCK_SIZE < min_x) {
		return;
	}
	draw_picture(ROW_CANVAS, 0xfa, IS_DRAW);
	autodown_x-=DRAW_BLOCK_SIZE;
	draw_picture(ROW_CANVAS, 0x1a, IS_DRAW);
}

void right()
{
	if (current_max_x+autodown_x+DRAW_BLOCK_SIZE > max_x) {
		return;
	}
	draw_picture(ROW_CANVAS, 0xfa, IS_DRAW);
	autodown_x+=DRAW_BLOCK_SIZE;
	draw_picture(ROW_CANVAS, 0x1a, IS_DRAW);
}

void down()
{
	if (current_max_y+autodown_y+DRAW_BLOCK_SIZE > max_y) {
		copy();
		reset_block();
		free(block_entry_p->arr);
		free(block_entry_p);
		exit(1);
		return;
	}
	draw_picture(ROW_CANVAS, 0xfa, IS_DRAW);
	autodown_y+=DRAW_BLOCK_SIZE;
	draw_picture(ROW_CANVAS, 0x1a, IS_DRAW);
}

void copy(){
	int i, j ,l, m, x, y , p, q;

	for (i = 0; i < ROW; i++) {
		for (j = 0; j < COL; j++) {
			x=i*DRAW_BLOCK_SIZE;
			y=j*DRAW_BLOCK_SIZE;
			for (l = 0; l < ROW_CANVAS; l++) {
				for (m = 0; m < COL_CANVAS; m++) {
					p=l*DRAW_BLOCK_SIZE+autodown_x;
					q=m*DRAW_BLOCK_SIZE+autodown_y;
					if(x == p && y ==q){
						russian_p[i][j]=1;
						draw_element(p, q, BLUE);
					}
				}
			}
		}
	}
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

	init();
	init_current_block();
	draw_picture(ROW_CANVAS, 0x1a, IS_DRAW);

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
	x+=600;
	//y+=200;
	for (i = 0; i < DRAW_BLOCK_SIZE; i++) {
		for (j = 0; j < DRAW_BLOCK_SIZE; j++) {
			fb_draw_point(x+j, y+i, color);
		}
	}
}

void sig_handler(int s)
{
	down();
	alarm(1);	
}
	
void verify(enum PIC_TYPE type, enum POSITION pos, int row_canvas,
	   	int *test_current_max_x, int *test_current_min_x, int *test_current_max_y){
	int i,j;
	int x,y;
	int max_x_i,  max_x_j, min_x_i,  min_x_j, max_y_i,  max_y_j;
	switch(type){
		case FUCK:
			switch(pos){
				case UP:
					tmp_arr[1][0] = 1;
					tmp_arr[0][1] = 1;
					tmp_arr[1][1] = 1;
					tmp_arr[2][1] = 1;
					min_x_i = 0; min_x_j = 1;
					max_x_i = 2; max_x_j = 1;
					max_y_i = 2; max_y_j = 1;
				break;

				case DOWN:
					tmp_arr[0][0] = 1;
					tmp_arr[1][0] = 1;
					tmp_arr[2][0] = 1;
					tmp_arr[1][1] = 1;
					min_x_i = 0; min_x_j = 0;
					max_x_i = 2; max_x_j = 0;
					max_y_i = 1; max_y_j = 1;
				break;

				case LEFT:
					tmp_arr[1][0] = 1;
					tmp_arr[0][1] = 1;
					tmp_arr[1][1] = 1;
					tmp_arr[1][2] = 1;
					min_x_i = 0; min_x_j = 1;
					max_x_i = 1; max_x_j = 0;
					max_y_i = 1; max_y_j = 2;
				break;

				case RIGHT:
					tmp_arr[1][0] = 1;
					tmp_arr[1][2] = 1;
					tmp_arr[1][1] = 1;
					tmp_arr[2][1] = 1;
					min_x_i = 1; min_x_j = 0;
					max_x_i = 2; max_x_j = 1;
					max_y_i = 1; max_y_j = 2;
				break;

				default:
				break;
			}
		break;
		default:break;
	}

	for (i = 0; i < row_canvas; i++) {
		for (j = 0; j < COL_CANVAS; j++) {
			if (tmp_arr[i][j] == 1 ) {
				x = i*DRAW_BLOCK_SIZE;
				y = j*DRAW_BLOCK_SIZE;
				if(max_x_i == i && max_x_j == j){
					*test_current_max_x = x;
				}
				if(min_x_i == i && min_x_j == j){
					*test_current_min_x = x;
				}
				if(max_y_i == i && max_y_j == j){
					*test_current_max_y =y;
				}
				//reset!!
				tmp_arr[i][j] = 0;
			}
		}
	}

	return;
}

void draw_picture(int row_canvas, int color, int is_draw){
	int i,j;
	int x,y;
	int max_x_i,  max_x_j, min_x_i,  min_x_j, max_y_i,  max_y_j;

	switch(block_entry_p->type){
		case FUCK:
			switch(block_entry_p->pos){
				case UP:
					block_entry_p->arr[1][0] = 1;
					block_entry_p->arr[0][1] = 1;
					block_entry_p->arr[1][1] = 1;
					block_entry_p->arr[2][1] = 1;
					min_x_i = 0; min_x_j = 1;
					max_x_i = 2; max_x_j = 1;
					max_y_i = 2; max_y_j = 1;
				break;

				case DOWN:
					block_entry_p->arr[0][0] = 1;
					block_entry_p->arr[1][0] = 1;
					block_entry_p->arr[2][0] = 1;
					block_entry_p->arr[1][1] = 1;
					min_x_i = 0; min_x_j = 0;
					max_x_i = 2; max_x_j = 0;
					max_y_i = 1; max_y_j = 1;
				break;

				case LEFT:
					block_entry_p->arr[1][0] = 1;
					block_entry_p->arr[0][1] = 1;
					block_entry_p->arr[1][1] = 1;
					block_entry_p->arr[1][2] = 1;
					min_x_i = 0; min_x_j = 1;
					max_x_i = 1; max_x_j = 0;
					max_y_i = 1; max_y_j = 2;
				break;

				case RIGHT:
					block_entry_p->arr[1][0] = 1;
					block_entry_p->arr[1][2] = 1;
					block_entry_p->arr[1][1] = 1;
					block_entry_p->arr[2][1] = 1;
					min_x_i = 1; min_x_j = 0;
					max_x_i = 2; max_x_j = 1;
					max_y_i = 1; max_y_j = 2;
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
				if(max_x_i == i && max_x_j == j){
					current_max_x = x;
				}
				if(min_x_i == i && min_x_j == j){
					current_min_x = x;
				}
				if(max_y_i == i && max_y_j == j){
				
					current_max_y =y;
				}
				if (is_draw == IS_DRAW) {
					draw_element(x+autodown_x, y+autodown_y, color);
				}
			}
		}
	}
	//printf(" current_max_x %d  current_max_y %d current_min_x %d \n", current_max_x, current_max_y, current_min_x);

}

void init()
{
	int i,j;
	int x,y;
	int is_init = 0;

	russian_p = calloc(COL*ROW, sizeof(int));

	for (i = 0; i < ROW; i++) {
		for (j = 0; j < COL; j++) {
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
	//printf("max_x %d min_x %d max_y %d min_y %d\n", max_x, min_x, max_y, min_y);

}

