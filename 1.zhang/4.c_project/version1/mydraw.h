#ifndef MY_DRAW_H
#define MY_DRAW_H

#include "fb_draw.h"

#define	WHITE		0xff
#define	BACK		0xfa
#define COLOR		0xaf
#define XZERO		400
#define YZERO		200
#define	SIZE		20	
#define XSIZE		300
#define YSIZE		400
#define XNUM		XSIZE/SIZE
#define YNUM		YSIZE/SIZE


void draw_element(int x, int y, int color);
void draw_back(void);
void draw_grid(void);
void draw_fresh(char (*p)[XNUM]);
void draw_flag(int x, int y, int type, char (*p)[XNUM]);
void type_chg(int *type);
void change(int x, int y, int *type, char (*p)[XNUM]);
int is_oneline(char (*p)[XNUM]);
void deleteone(char (*p)[XNUM]);
int gameover(char (*p)[XNUM]);

#endif
