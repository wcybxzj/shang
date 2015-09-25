#include "fb_draw.h"
#include "mydraw.h"



void draw_element(int x, int y, int color)
{
	int px = x*SIZE;
	int py = y*SIZE;
	int i, j;	

	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			fb_draw_point(px+j, py+i, color);
		}
	}
}


void draw_fresh(char (*p)[32])
{
	int i, j;

	for(i = 0; i < 24; i++) {
		for(j = 0; j < 32; j++) {
			if(p[i][j]) {
				draw_element(j, i, COLOR);	
			}else {
				draw_element(j, i, BLACK);
			}
		}
	}
}


void set_arra(int x, int y, char (*p)[32])
{
	p[y][x]		= 1;
	p[y+1][x]	= 1;
	p[y+1][x+1] = 1;
	p[y+1][x-1] = 1;
}

void reset_arra(int x, int y, char (*p)[32])
{
	p[y][x]		= 0;
	p[y+1][x]	= 0 ;
	p[y+1][x+1] = 0;
	p[y+1][x-1] = 0;
}

void set_arrb(int x, int y, char (*p)[32])
{
	p[y][x]		= 1;
	p[y+1][x]	= 1;
	p[y+1][x+1] = 1;
	p[y+2][x]   = 1;
}

void reset_arrb(int x, int y, char (*p)[32])
{
	p[y][x]		= 0;
	p[y+1][x]	= 0;
	p[y+1][x+1] = 0;
	p[y+2][x]   = 0;
}

void set_arrc(int x, int y, char (*p)[32])
{
	p[y][x]		= 1;
	p[y+1][x]	= 1;
	p[y+1][x-1] = 1;
	p[y+2][x]   = 1;
}

void reset_arrc(int x, int y, char (*p)[32])
{
	p[y][x]		= 0;
	p[y+1][x]	= 0;
	p[y+1][x-1] = 0;
	p[y+2][x]   = 0;
}

int is_oneline(char (*p)[32])
{
	int i, ret = 1;

	for(i = 0; i < 32; i++) {
		ret &= p[23][i];
	}

	return ret;
}



