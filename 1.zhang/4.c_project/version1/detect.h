#ifndef MY_DETECT_H
#define MY_DETECT_H

#include "mydraw.h"

int canleft(int x, int y, int type, char (*p)[XNUM]);
int canright(int x, int y, int type, char (*p)[XNUM]);
int candown(int x, int y, int type, char (*p)[XNUM]);
int canchange(int x, int y, int type, char (*p)[XNUM]);


#endif
