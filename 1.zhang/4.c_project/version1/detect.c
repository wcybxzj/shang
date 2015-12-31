#include "detect.h"
#include "mydraw.h"


int canleft(int x, int y, int type, char (*p)[XNUM])
{
	switch(type) {
		case 0:
			if((x-1) > 0 && !p[y][x-1] && !p[y+1][x-2])
			   	return 1;
			else 
				return 0;
		case 1:
			if(x > 0&&!p[y][x-1]&&!p[y+1][x-1]&&!p[y+2][x-1])
				return 1;
			else
				return 0;
		case 2:
			if((x-1)>0&&!p[y][x-1]&&!p[y+1][x-2]&&!p[y+2][x-1])
				return 1;
			else 
				return 0;
		case 3:
			if((x-1) > 0 && !p[y][x-2] && !p[y+1][x-1])
				return 1;
			else 
				return 0;
		case 4:
			if((x-1) > 0 && !p[y][x-2] && !p[y+1][x-1])
				return 1;
			else 
				return 0;
		case 5:
			if((x-1)>0&&!p[y][x-1]&&!p[y+1][x-2]&&!p[y+2][x-2])
				return 1;
			else 
				return 0;
		case 6:
			if((x-1)>0&&!p[y][x-1]&&!p[y+1][x-2])
				return 1;
			else 
				return 0;
		case 7:
			if(x>0&&!p[y][x-1]&&!p[y+1][x-1]&&!p[y+2][x])
				return 1;
			else
				return 0;
		case 8:
			if(x>0&&!p[y][x-1]&&!p[y+1][x-1])
				return 1;
			else 
				return 0;
		case 9:
			if((x-1)>0&&!p[y][x-1]&&!p[y+1][x-1]&&!p[y+2][x-2])
				return 1;
			else
				return 0;
		case 10:
			if(x>0&&!p[y][x-1]&&!p[y+1][x+1])
				return 1;
			else
				return 0;
		case 11:
			if(x>0&&!p[y][x-1]&&!p[y+1][x-1]&&!p[y+2][x-1])
				return 1;
			else
				return 0;
		case 12:
			if(x>0&&!p[y][x-1]&&!p[y+1][x-1])
				return 1;
			else
				return 0;
		case 13:
			if(x>0&&!p[y][x-1]&&!p[y+1][x-1]&&!p[y+2][x-1]&&!p[y+3][x-1])
				return 1;
			else
				return 0;
		case 14:
			if(x > 0 && !p[y][x])
				return 1;
			else
				return 0;
		default:
			break;
	}
}

int canright(int x, int y, int type, char (*p)[XNUM])
{
	switch(type) {
		case 0:
			if((x+1) < XNUM-1 && !p[y][x+1] && !p[y+1][x+2])
				return 1;
			else
				return 0;
		case 1:
			if((x+1)<XNUM-1&&!p[y][x+1]&&!p[y+1][x+2]&&!p[y+2][x+1])
				return 1;
			else 
				return 0;
		case 2:
			if(x < XNUM-1&&!p[y][x+1]&&!p[y+1][x+1]&&!p[y+2][x+1])
				return 1;
			else 
				return 0;
		case 3:
			if((x+1) < XNUM-1 && !p[y][x+2] && !p[y+1][x+1])
				return 1;
			else 
				return 0;
		case 4:
			if((x+1) < XNUM-1 && !p[y][x+1] && !p[y+1][x+2])
				return 1;
			else 
				return 0;
		case 5:
			if(x < XNUM-1 &&!p[y][x+1]&&!p[y+1][x+1]&&!p[y+2][x])
				return 1;
			else 
				return 0;
		case 6:
			if((x+1)<XNUM-1&&!p[y][x+2]&&!p[y+1][x+1])
				return 1;
			else 
				return 0;
		case 7:
			if((x+1)<XNUM-1&&!p[y][x+1]&&!p[y+1][x+2]&&!p[y+2][x+2])
				return 1;
			else
				return 0;
		case 8:
			if((x+2)<XNUM-1&&!p[y][x+1]&&!p[y+1][x+3])
				return 1;
			else 
				return 0;
		case 9:
			if(x<XNUM-1&&!p[y][x+1]&&!p[y+1][x+1]&&!p[y+2][x+1])
				return 1;
			else
				return 0;
		case 10:
			if((x+2)<XNUM-1&&!p[y][x+3]&&!p[y+1][x+3])
				return 1;
			else
				return 0;
		case 11:
			if((x+1)<XNUM-1&&!p[y][x+2]&&!p[y+1][x+1]&&!p[y+2][x+1])
				return 1;
			else
				return 0;
		case 12:
			if((x+1)<XNUM-1&&!p[y][x+2]&&!p[y+1][x+2])
				return 1;
			else
				return 0;
		case 13:
			if(x<XNUM-1&&!p[y][x+1]&&!p[y+1][x+1]&&!p[y+2][x+1]&&!p[y+3][x+1])
				return 1;
			else
				return 0;
		case 14:
			if((x+3) < XNUM-1 && !p[y][x+4])
				return 1;
			else
				return 0;
		default:
			break;
	}
}

int candown(int x, int y, int type, char (*p)[XNUM])
{
	switch(type) {
		case 0:
			if((y+1) < YNUM-1&&!p[y+2][x]&&!p[y+2][x-1]&&!p[y+2][x+1])
				return 1;
			else
				return 0;
		case 1:
			if((y+2) < YNUM-1&&!p[y+3][x]&&!p[y+2][x+1])
				return 1;
			else
				return 0;
		case 2:
			if((y+2) < YNUM-1&&!p[y+3][x]&&!p[y+2][x-1])
				return 1;
			else
				return 0;
		case 3:
			if((y+1)<YNUM-1 && !p[y+1][x-1] && !p[y+2][x] && !p[y+1][x+1])
				return 1;
			else 
				return 0;
		case 4:
			if((y+1)<YNUM-1 && !p[y+1][x-1] && !p[y+2][x] && !p[y+2][x+1])
				return 1;
			else 
				return 0;
		case 5:
			if((y+2)<YNUM-1&&!p[y+3][x-1]&&!p[y+2][x])
				return 1;
			else 
				return 0;
		case 6:
			if((y+1)<YNUM-1&&!p[y+2][x-1]&&!p[y+2][x]&&!p[y+1][x+1])
				return 1;
			else 
				return 0;
		case 7:
			if((y+2)<YNUM-1&&!p[y+2][x]&&!p[y+3][x+1])
				return 1;
			else
				return 0;
		case 8:
			if((y+1)<YNUM-1&&!p[y+2][x]&&!p[y+2][x+1]&&!p[y+2][x+2])
				return 1;
			else 
				return 0;
		case 9:
			if((y+2)<YNUM-1&&!p[y+3][x-1]&&!p[y+3][x])
				return 1;
			else
				return 0;
		case 10:
			if((y+1)<YNUM-1&&!p[y+1][x]&&!p[y+1][x+1]&&!p[y+2][x+2])
				return 1;
			else
				return 0;
		case 11:
			if((y+2)<YNUM-1&&!p[y+3][x]&&!p[y+1][x+1])
				return 1;
			else
				return 0;
		case 12:
			if((y+1)<YNUM-1&&!p[y+2][x]&&!p[y+2][x+1])
				return 1;
			else
				return 0;
		case 13:
			if((y+3)<YNUM-1&&!p[y+4][x])
				return 1;
			else
				return 0;
		case 14:
			if(y<YNUM-1&&!p[y+1][x]&&!p[y+1][x+1]&&!p[y+1][x+2]&&!p[y+1][x+3])
				return 1;
			else
				return 0;
		default:
			break;
	}
}

int canchange(int x, int y, int type, char (*p)[XNUM])
{
	switch(type) {
		case 0:
			if((y+1) < YNUM-1 && !p[y+2][x])
				return 1;
			else 
				return 0;
		case 1:
			if(x > 0 && !p[y+1][x-1])
				return 1;
			else
				return 0;
		case 2:
			if(x < XNUM-1 && !p[y+1][x+1])
				return 1;
			else
				return 0;
		case 3:
			if(!p[y+1][x-1] && !p[y+1][x+1])
				return 1;
			else 
				return 0;
		case 4:
			if((y+1) < YNUM-1 && !p[y+1][x-1] && !p[y+2][x-1])
				return 1;
			else 
				return 0;
		case 5:
			if(x < XNUM-1 && !p[y][x-1] && !p[y+1][x+1])
				return 1;
			else 
				return 0;
		case 6:
			if((y+1) < YNUM-1 && !p[y+1][x+1] && !p[y+2][x+1])
				return 1;
			else 
				return 0;
		case 7:
			if(x > 0 && !p[y][x+1] && !p[y+1][x+1])
				return 1;
			else 
				return 0;
		case 8:
			if(x > 0 && (y+1) < YNUM-1 && !p[y+2][x-1] && !p[y+2][x])
				return 1;
			else 
				return 0;
		case 9:
			if((x+1)<XNUM-1&&!p[y][x+1]&&!p[y][x+2]&&!p[y+1][x+2])
				return 1;
			else
				return 0;
		case 10:
			if((y+1) < YNUM-1 && !p[y+1][x] && !p[y+2][x])
				return 1;
			else
				return 0;
		case 11:
			if((x+1) < XNUM-1 && !p[y+1][x+1] && !p[y+1][x+2])
				return 1;
			else 
				return 0;
		case 12:
			return 1;
		case 13:
			if((x+2)<XNUM-1&&!p[y][x+1]&&!p[y][x+2]&&!p[y][x+3])
				return 1;
			else
				return 0;
		case 14:
			if((y+2)<YNUM-1&&!p[y+1][x]&&!p[y+2][x]&&!p[y+3][x])
				return 1;
			else
				return 0;
		default:
			break;
	}
}









