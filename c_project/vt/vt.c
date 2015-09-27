#include <stdio.h>
#include <unistd.h>

void vt_draw_point(int x, int y, int color){
	char pos[30]={};
	char col[30]={};
	snprintf(col, 30, "\33[%dm", color);

	int i, j;
	for (i = 0; i < 2; i++) {
		snprintf(pos, 30, "\33[%d;%dH", x, y);
		printf("%s", pos);
		x+=1;
		for (j = 0; j < 5; j++) {
			printf("%s %d %d", col,i,j);
			//printf("%s", col);
			fflush(0);
			//sleep(1);
		}
		printf("\33[0m");
	}
}

int main(int argc, const char *argv[])
{

	//printf("\33[2J");
	//char pos[30];
	//snprintf(pos, 30, "\33[%d;%dH",0,0);
	//printf("%s\n", pos);
	//printf("\33[42m[ ]");
	//printf("\33[0m");

	printf("\33[2J");
	vt_draw_point(1, 1, 43);
	vt_draw_point(5, 5, 42);
	//vt_draw_point(0, 5, 43);
	//vt_draw_point(0, 10, 44);

	//printf("\33[2J");
	//printf("\33[0;0H");
	//printf("\33[42m[ ]");
	//printf("\33[0m");

	return 0;
}

