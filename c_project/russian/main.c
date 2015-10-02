#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#include "fb_draw.h"

#define PINK 0xfa
#define BLUE 0x1a
#define YELLOW 0xaf

#define COL_CANVAS 4
#define ROW_CANVAS 4

#define ROW 20
#define COL 10
#define DEGREE 2

#define DRAW_BLOCK_SIZE 20

enum ALLOW_TYPE{NOT_ALLOW, ALLOW};
enum MOVE_VERIFY_TYPE{VERRIFY_DOWN, VERRIFY_LEFT, VERRIFY_RIGHT};
enum PIC_TYPE{BACK_GROUP,GUN,RECT,FUCK,LIANG,ZUO,YOU};
enum POSITION{UP,RIGHT,DOWN,LEFT};
enum IS_DRAW{IS_DRAW, NOT_DRAW};

typedef struct{
	enum PIC_TYPE type;
	enum POSITION pos;
	char (*arr)[COL_CANVAS];
}block,*block_entry_pointer;

typedef struct st_top_i_j{
	int i;
	int j;
}top_i_j;

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
enum ALLOW_TYPE verify(enum PIC_TYPE type, enum POSITION pos, int row_canvas);
void copy();
int is_stop(enum MOVE_VERIFY_TYPE move_verify_type);

void reset_block()
{
	int i,j;
	int x,y;
	for (i = 0; i < ROW_CANVAS; i++) {
		for (j = 0; j < COL_CANVAS; j++) {
			if ( block_entry_p->arr[i][j] == 1 ) {
				block_entry_p->arr[i][j] = 0;
				x = j * DRAW_BLOCK_SIZE;
				y = i * DRAW_BLOCK_SIZE;
				draw_element(x+autodown_x, y+autodown_y, PINK);
			}
		}
	}
}

void change()
{
	enum ALLOW_TYPE is_allow;
	enum POSITION next_pos = (block_entry_p->pos+1)%4;

	is_allow = verify(block_entry_p->type, next_pos,ROW_CANVAS);
	if(is_allow == ALLOW){
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
	autodown_y=0;
	autodown_x=0;
}

void left()
{
	if (current_min_x+autodown_x-DRAW_BLOCK_SIZE < min_x || is_stop(VERRIFY_LEFT)) {
		return;
	}
	draw_picture(ROW_CANVAS, 0xfa, IS_DRAW);
	autodown_x-=DRAW_BLOCK_SIZE;
	draw_picture(ROW_CANVAS, 0x1a, IS_DRAW);
}

void right()
{
	if (current_max_x+autodown_x+DRAW_BLOCK_SIZE > max_x|| is_stop(VERRIFY_RIGHT)) {
		return;
	}
	draw_picture(ROW_CANVAS, 0xfa, IS_DRAW);
   	autodown_x+=DRAW_BLOCK_SIZE;
   	draw_picture(ROW_CANVAS, 0x1a, IS_DRAW);
}

void get_russian(){
	int i, j;
	int x, y;
	for (i = 0; i < ROW; i++) {
		for (j = 0; j < COL; j++) {
			x=j*DRAW_BLOCK_SIZE;
			y=i*DRAW_BLOCK_SIZE;
			printf("i %d, j %d, x %d, y %d, value %d |||", i, j, x, y, russian_p[i][j]);
		}
		printf("\n");
	}
}

void get_current_block(){
	int l, m;
	int p, q;
	for (l = 0; l < ROW_CANVAS; l++) {
		for (m = 0; m < COL_CANVAS; m++) {
			p=m*DRAW_BLOCK_SIZE+autodown_x;
			q=l*DRAW_BLOCK_SIZE+autodown_y;
			printf("l %d,m %d, p %d, q %d ,value %d|||", l, m, p, q, block_entry_p->arr[l][m]);
		}
		printf("\n");
	}
}

int is_stop(enum MOVE_VERIFY_TYPE type){
	int i, j, x, y;
	int l, m, p, q;
	int origin_q;
	int left_p, right_p;
	for (l = 0; l < ROW_CANVAS; l++) {
		for (m = 0; m < COL_CANVAS; m++) {
			if ( block_entry_p->arr[l][m] == 1 ) {
				//printf("00000---- l %d m %d\n",l ,m);
				p = m*DRAW_BLOCK_SIZE+autodown_x;
				origin_q = l*DRAW_BLOCK_SIZE+autodown_y;
				q = origin_q+DRAW_BLOCK_SIZE;
				left_p = p-DRAW_BLOCK_SIZE;
				right_p = p+DRAW_BLOCK_SIZE;

				for (i = 0; i < ROW; i++) {
					for (j = 0; j < COL; j++) {
						x=j*DRAW_BLOCK_SIZE;
						y=i*DRAW_BLOCK_SIZE;
						if(type == VERRIFY_DOWN){
							if(p==x && q==y){
								if(russian_p[i][j]==1){
									return 1;
								}
							}
						}else if(type == VERRIFY_LEFT){
							if( left_p==x && origin_q==y){
								if(russian_p[i][j]==1){
									return 1;
								}
							}
						}else if(type == VERRIFY_RIGHT){
							if( right_p==x && origin_q==y){
								if(russian_p[i][j]==1){
									return 1;
								}
							}
						}
					}
				}
			}
		}
	}
	return 0;
}

void down()
{
	if (current_max_y+autodown_y+DRAW_BLOCK_SIZE > max_y || is_stop(VERRIFY_DOWN)) {
		//get_russian();
		//printf("--------11111111111111-------------\n");
		//get_current_block();
		//printf("--------22222222222222-------------\n");
		copy();
		//printf("-------333333333333--------------\n");
		//get_russian();
		//sleep(5);

		free(block_entry_p->arr);
		free(block_entry_p);


		init_current_block();
		draw_picture(ROW_CANVAS, 0x1a, IS_DRAW);

		//exit(1);
		return;
	}

	draw_picture(ROW_CANVAS, 0xfa, IS_DRAW);
	autodown_y+=DRAW_BLOCK_SIZE;
	draw_picture(ROW_CANVAS, 0x1a, IS_DRAW);
}


void copy(){
	int k;
	int i, j ,l, m, x, y, z , p, q;
	int is_game_over = 0;

	//set top level i j for copy gamever
	top_i_j tmp;
	top_i_j i_j_arr[COL];
	for (k = 0; k < COL; k++) {
		tmp.i = 0;
		tmp.j = k;
		i_j_arr[k] =tmp;
	}

	for (i = 0; i < ROW; i++) {
		for (j = 0; j < COL; j++) {
			x=j*DRAW_BLOCK_SIZE;
			y=i*DRAW_BLOCK_SIZE;
			for (l = 0; l < ROW_CANVAS; l++) {
				for (m = 0; m < COL_CANVAS; m++) {
					p=m*DRAW_BLOCK_SIZE+autodown_x;
					q=l*DRAW_BLOCK_SIZE+autodown_y;
					if(x == p && y ==q){
						if(block_entry_p->arr[l][m]==1){
							//printf("wwwwwwwwwwwwwwwwwwwwwwww i %d j %d\n", i, j);
							russian_p[i][j]=1;
							draw_element(p, q, YELLOW);
							for (k = 0; k < COL; k++) {
								if (i==i_j_arr[k].i && j ==i_j_arr[k].j) {
									is_game_over=1;
								}
							}
						}
					}
				}
			}
		}
	}

	//delete complete block
	int delete_num = 0;
	int last_delete_row = 0;
	for (i = ROW-1; i > 0; i--) {
		for (j = 0; j < COL; j++) {
			if (russian_p[i][j] == 0) {
				break;
			}
			if (j==COL-1) {
				for (k = 0; k < COL; k++) {
					x=k*DRAW_BLOCK_SIZE;
					y=i*DRAW_BLOCK_SIZE;
					draw_element(x, y, PINK);
					russian_p[i][k] = 0;
					printf("delete i %d k %d\n",i,k);
				}
				delete_num++;
				last_delete_row = i;
				printf("---------------------------------\n");
				sleep(2);
			}
		}
	}
	printf("delete_num %d\n", delete_num);

	//after delete move down
	if (delete_num) {
		for (j = 0; j < COL; j++) {
			for (i = last_delete_row-1; i >= 0 ; i--) {
				x = j * DRAW_BLOCK_SIZE;
				y = i * DRAW_BLOCK_SIZE;
				z = (i+delete_num) * DRAW_BLOCK_SIZE;
				if (russian_p[i][j] == 1) {
					draw_element(x, y, PINK);
					russian_p[i][j] = 0;
					//sleep(2);
					printf("z %d y %d\n",z, y);
					draw_element(x, z, YELLOW);
					russian_p[i+delete_num][j] = 1;
				}
			}
		}
	}

	if (is_game_over) {
		printf("game over!!!");
		exit(1);
	}
}

int main(int argc, const char *argv[])
{
	system("clear");
	int ret,ch;
	ret = fb_open();	
	//draw_element(100, 100, 0x75);
	//draw_element(150, 100, PINK);
	//exit(1);

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
	x+=800;
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

enum ALLOW_TYPE verify(enum PIC_TYPE type, enum POSITION pos, int row_canvas){
	int test_current_max_x, test_current_min_x, test_current_max_y;
	int i, j, l, m;
	int x, y, p, q, real_x, real_y;
	int max_x_i,  max_x_j, min_x_i,  min_x_j, max_y_i,  max_y_j;
	switch(type){
		case FUCK:
			switch(pos){
				case UP:
					tmp_arr[0][1] = 1;
					tmp_arr[1][0] = 1;
					tmp_arr[1][1] = 1;
					tmp_arr[1][2] = 1;
					min_x_i = 1; min_x_j = 0;
					max_x_i = 1; max_x_j = 2;
					max_y_i = 1; max_y_j = 2;
					break;
				case DOWN:
					tmp_arr[0][0] = 1;
					tmp_arr[0][1] = 1;
					tmp_arr[0][2] = 1;
					tmp_arr[1][1] = 1;
					min_x_i = 0; min_x_j = 0;
					max_x_i = 0; max_x_j = 2;
					max_y_i = 1; max_y_j = 1;
					break;
				case LEFT:
					tmp_arr[1][0] = 1;
					tmp_arr[0][1] = 1;
					tmp_arr[1][1] = 1;
					tmp_arr[2][1] = 1;
					min_x_i = 1; min_x_j = 0;
					max_x_i = 0; max_x_j = 1;
					max_y_i = 2; max_y_j = 1;
					break;
				case RIGHT:
					tmp_arr[0][1] = 1;
					tmp_arr[1][1] = 1;
					tmp_arr[1][2] = 1;
					tmp_arr[2][1] = 1;
					min_x_i = 0; min_x_j = 1;
					max_x_i = 1; max_x_j = 2;
					max_y_i = 2; max_y_j = 1;
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
				x = j*DRAW_BLOCK_SIZE;
				y = i*DRAW_BLOCK_SIZE;
				real_x = x+autodown_x;
				real_y = y+autodown_y;
				if(max_x_i == i && max_x_j == j){
					test_current_max_x = x;
				}
				if(min_x_i == i && min_x_j == j){
					test_current_min_x = x;
				}
				if(max_y_i == i && max_y_j == j){
					test_current_max_y =y;
				}

				//reset!!
				tmp_arr[i][j] = 0;

				//
				for (l = 0; l < ROW; l++) {
					for (m = 0; m < COL; m++) {
						p=m*DRAW_BLOCK_SIZE;
						q=l*DRAW_BLOCK_SIZE;
						if(p==real_x && q==real_y){
							if(russian_p[l][m]==1){
								return NOT_ALLOW;
							}
						}
					}
				}
			}
		}
	}

	if (type == FUCK) {
		if (pos == DOWN) {
			if (test_current_min_x + autodown_x < min_x) {
				return NOT_ALLOW;
			}
		}else if(pos == UP){
			if (test_current_max_x + autodown_x > max_x) {
				return NOT_ALLOW;
			}
		}else if(pos == RIGHT){
			if (test_current_max_y + autodown_y > max_y) {
				return NOT_ALLOW;
			}
		}
	}

	return ALLOW;
}

void draw_picture(int row_canvas, int color, int is_draw){
	int i,j;
	int x,y;
	int max_x_i,  max_x_j, min_x_i,  min_x_j, max_y_i,  max_y_j;

	switch(block_entry_p->type){
		case FUCK:
			switch(block_entry_p->pos){

				case UP:
					block_entry_p->arr[0][1] = 1;
					block_entry_p->arr[1][0] = 1;
					block_entry_p->arr[1][1] = 1;
					block_entry_p->arr[1][2] = 1;
					min_x_i = 1; min_x_j = 0;
					max_x_i = 1; max_x_j = 2;
					max_y_i = 1; max_y_j = 2;
					break;
				case DOWN:
					block_entry_p->arr[0][0] = 1;
					block_entry_p->arr[0][1] = 1;
					block_entry_p->arr[0][2] = 1;
					block_entry_p->arr[1][1] = 1;
					min_x_i = 0; min_x_j = 0;
					max_x_i = 0; max_x_j = 2;
					max_y_i = 1; max_y_j = 1;
					break;
				case LEFT:
					block_entry_p->arr[1][0] = 1;
					block_entry_p->arr[0][1] = 1;
					block_entry_p->arr[1][1] = 1;
					block_entry_p->arr[2][1] = 1;
					min_x_i = 1; min_x_j = 0;
					max_x_i = 0; max_x_j = 1;
					max_y_i = 2; max_y_j = 1;
					break;
				case RIGHT:
					block_entry_p->arr[0][1] = 1;
					block_entry_p->arr[1][1] = 1;
					block_entry_p->arr[1][2] = 1;
					block_entry_p->arr[2][1] = 1;
					min_x_i = 0; min_x_j = 1;
					max_x_i = 1; max_x_j = 2;
					max_y_i = 2; max_y_j = 1;
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
				x = j*DRAW_BLOCK_SIZE;
				y = i*DRAW_BLOCK_SIZE;
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
					//sleep(1);
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

	for (i = 0; i < ROW; i++) {//20
		for (j = 0; j < COL; j++) {//10
			x=j*DRAW_BLOCK_SIZE;
			y=i*DRAW_BLOCK_SIZE;
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
			//sleep(1);
		}
	}
	//printf("max_x %d min_x %d max_y %d min_y %d\n", max_x, min_x, max_y, min_y);


}
