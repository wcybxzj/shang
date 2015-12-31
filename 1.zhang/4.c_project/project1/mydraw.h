#ifndef MY_DRAW_H
#define MY_DRAW_H

#include "fb_draw.h"

#define	WHITE		0xff
#define	BLACK		0x00
#define COLOR		0xfa
#define	SIZE		32
#define XSIZE		xres()
#define YSIZE		yres()
#define XNUM		XSIZE/SIZE
#define YNUM		YSIZE/SIZE


void draw_element(int x, int y, int color);
void draw_back(void);
void draw_fresh(char (*p)[32]);
void set_arra(int i, int j, char (*p)[32]);
void reset_arra(int i, int j, char (*p)[32]);
int is_oneline(char (*p)[32]);

#endif
