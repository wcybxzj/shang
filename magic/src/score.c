#include "../include/ini.h"
extern int state[10][9];//每个方格的状态
extern int pos[10][9][2];//每个方格的左上角的坐标
extern int sco;


int judge(){
	int i,j,sum;
	for(i=0;i<10;i++){
		sum=0;
		for(j=0;j<9;j++)
			sum+=state[i][j];
		if(sum==9)return i;
	}
	return 100;
}
void score(){
	int i=judge();
	int j;
	while(i!=100)
	{
		for(;i>0;i--)
			for(j=0;j<9;j++)
				state[i][j]=state[(i-1)][j];
		for(j=0;j<9;j++)state[0][j]=0;
		sco+=100;
		printf("%5d",sco);
		i=judge();
	}
	return;
}

