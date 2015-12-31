#include "fb_draw.h"
#include "mydraw.h"



void draw_element(int x, int y, int color)
{
	int px = x*SIZE+XZERO;
	int py = y*SIZE+YZERO;
	int i, j;	

	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			fb_draw_point(px+j, py+i, color);
		}
	}
}

void draw_grid(void)
{
	int i, j;

	for(i = XZERO; i <= XZERO+XSIZE; i++) {
		for(j = YZERO; j <= YZERO+YSIZE; j+=SIZE) {
			fb_draw_point(i, j, WHITE);
		}
	}
	for(i = XZERO; i <= XZERO+XSIZE; i+=SIZE) {
		for(j = YZERO; j <= YZERO+YSIZE; j++) {
			fb_draw_point(i, j, WHITE);
		}
	}
}

void draw_back(void)
{
	int i, j;

	for(i = XZERO; i <= XZERO+XSIZE; i++) {
		for(j = YZERO; j <= YZERO+YSIZE; j++) {
			fb_draw_point(i, j, BACK);
		}
	}
}

void draw_fresh(char (*p)[XNUM])
{
	int i, j;

	for(i = 0; i < YNUM; i++) {
		for(j = 0; j < XNUM; j++) {
			if(p[i][j]) {
				draw_element(j, i, COLOR);	
			}else {
				draw_element(j, i, BACK);
			}
		}
	}
}

void draw_flag(int x, int y, int type, char (*p)[XNUM])
{
	switch(type) {
		case 0:
			p[y][x]=1;p[y+1][x]=1;p[y+1][x+1]=1;p[y+1][x-1]=1;
			break;
		case 1:
			p[y][x]=1;p[y+1][x]=1;p[y+1][x+1]=1;p[y+2][x]=1;
			break;
		case 2:
			p[y][x]=1;p[y+1][x]=1;p[y+1][x-1]=1;p[y+2][x]=1;
			break;
		case 3:
			p[y][x]=1;p[y][x-1]=1;p[y][x+1]=1;p[y+1][x]=1;
			break;
		case 4:
			p[y][x]=1;p[y][x-1]=1;p[y+1][x]=1;p[y+1][x+1]=1;
			break;
		case 5:
			p[y][x]=1;p[y+1][x-1]=1;p[y+1][x]=1;p[y+2][x-1]=1;
			break;
		case 6:
			p[y][x]=1;p[y][x+1]=1;p[y+1][x-1]=1;p[y+1][x]=1;
			break;
		case 7:
			p[y][x]=1;p[y+1][x]=1;p[y+1][x+1]=1;p[y+2][x+1]=1;
			break;
		case 8:
			p[y][x]=1;p[y+1][x]=1;p[y+1][x+1]=1;p[y+1][x+2]=1;
			break;
		case 9:
			p[y][x]=1;p[y+1][x]=1;p[y+2][x-1]=1;p[y+2][x]=1;
			break;
		case 10:
			p[y][x]=1;p[y][x+1]=1;p[y][x+2]=1;p[y+1][x+2]=1;
			break;
		case 11:
			p[y][x]=1;p[y][x+1]=1;p[y+1][x]=1;p[y+2][x]=1;
			break;
		case 12:
			p[y][x]=1;p[y][x+1]=1;p[y+1][x]=1;p[y+1][x+1]=1;
			break;
		case 13:
			p[y][x]=1;p[y+1][x]=1;p[y+2][x]=1;p[y+3][x]=1;
			break;
		case 14:
			p[y][x]=1;p[y][x+1]=1;p[y][x+2]=1;p[y][x+3]=1;
		default:
			break;
	}
}

void draw_type(int x, int y, int type, int color)
{
	switch(type) {
		case 0:                           //      ____
			draw_element(x, y, color);    //     |    |
			draw_element(x, y+1, color);  // ____|    |____
			draw_element(x+1, y+1, color);//|              |
			draw_element(x-1, y+1, color);//|______________|
			break;                  
		case 1:                           
			draw_element(x, y, color);   
			draw_element(x, y+1, color); 
			draw_element(x+1, y+1, color);
			draw_element(x, y+2, color);  
			break;                   
		case 2:                           
			draw_element(x, y, color);    
			draw_element(x, y+1, color);  
			draw_element(x-1, y+1, color);
			draw_element(x, y+2, color);  
			break; 
		case 3:
			draw_element(x, y, color);
			draw_element(x, y+1, color);
			draw_element(x-1, y, color);
			draw_element(x+1, y, color);
			break;
		case 4:
			draw_element(x, y, color);
			draw_element(x-1, y, color);
			draw_element(x, y+1, color);
			draw_element(x+1, y+1, color);
			break;
		case 5:
			draw_element(x, y, color);
			draw_element(x, y+1, color);
			draw_element(x-1, y+1, color);
			draw_element(x-1, y+2, color);
			break;
		case 6:
			draw_element(x, y, color);
			draw_element(x+1, y, color);
			draw_element(x, y+1, color);
			draw_element(x-1, y+1, color);
			break;
		case 7:
			draw_element(x, y, color);
			draw_element(x, y+1, color);
			draw_element(x+1, y+1, color);
			draw_element(x+1, y+2, color);
			break;
		case 8:
			draw_element(x, y, color);
			draw_element(x, y+1, color);
			draw_element(x+1, y+1, color);
			draw_element(x+2, y+1, color);
			break;
		case 9:
			draw_element(x, y, color);
			draw_element(x, y+1, color);
			draw_element(x, y+2, color);
			draw_element(x-1, y+2, color);
			break;
		case 10:
			draw_element(x, y, color);
			draw_element(x+1, y, color);
			draw_element(x+2, y, color);
			draw_element(x+2, y+1, color);
			break;
		case 11:
			draw_element(x, y, color);
			draw_element(x+1, y, color);
			draw_element(x, y+1, color);
			draw_element(x, y+2, color);
			break;
		case 12:
			draw_element(x, y, color);
			draw_element(x+1, y, color);
			draw_element(x, y+1, color);
			draw_element(x+1, y+1, color);
			break;
		case 13:
			draw_element(x, y, color);
			draw_element(x, y+1, color);
			draw_element(x, y+2, color);
			draw_element(x, y+3, color);
			break;
		case 14:
			draw_element(x, y, color);
			draw_element(x+1, y, color);
			draw_element(x+2, y, color);
			draw_element(x+3, y, color);
			break;
		default:
			break;
	}
}

void type_chg(int *type)
{
	if(*type == 3)		*type = 0;
	else if(*type==5) 	*type = 4;
	else if(*type==7)	*type = 6;
	else if(*type==11)	*type = 8;
	else if(*type==12)	*type = 12;
	else if(*type==14)	*type = 13;
	else *type += 1;
}

int is_oneline(char (*p)[XNUM])
{
	int i, j, ret;

	for(j = YNUM-1; j >= 0; j--) {
		ret = 1;
		for(i = 0; i < XNUM; i++) {
			ret &= p[j][i];
		}
		if(ret)
			return j;
	}
	return 0;
}

void deleteone(char (*p)[XNUM])
{
	int i, j, ret;
	
	if(ret = is_oneline(p)) {
		for(i = 0; i < XNUM; i++) {
			for(j = ret; j > 0; j--) {
				p[j][i] = p[j-1][i];
			}
		}
		draw_fresh(p);
		draw_grid();
	}
}

int gameover(char (*p)[XNUM])
{
	int i, ret = 0;

	for(i = 0; i < XNUM; i++) {
		ret |= p[0][i];
	}
	return ret;
}





