#include "../include/ini.h"
extern int state[10][9];
extern int pos[10][9][2];
int can_left(int x,int y,int type){
	if(type==1){
		if((x-2)<=0)return 0;
		else if(state[y][(x-1)]||state[(y+1)][(x-3)])return 0;
		else return 1;
	}else if(type==2){
		if(x==0)return 0;
		else if(state[y][x]||state[(y+1)][x]||state[(y+2)][x])return 0;
		else return 1;
	}else if(type==3){
		if(x==0)return 0;
		else if(state[y][x]||state[(y+1)][x])return 0;
		else return 1;
	}else if(type==4){
		if(x==0)return 0;
		else if(state[y][(x-1)]||state[(y+1)][x]||state[(y+2)][x])return 0;
		else return 1;
	}else if(type==5){
		if(x==0)return 0;
		else if(state[y][(x-1)]||state[(y+1)][(x-1)])return 0;
		else return 1;
	}else if(type==6){
		if(x==0)return 0;
		else if(state[y][(x-1)]||state[(y+1)][(x-1)]||state[(y+2)][(x-1)])return 0;
		else return 1;
	}else if(type==7){
		if(x==0)return 0;
		else if(state[y][(x-1)]||state[(y+1)][(x+1)])return 0;
		else return 1;
	}else if(type==8){
		if(x<=1)return 0;
		else if(state[y][(x-1)]||state[(y+1)][(x-1)])return 0;
		else return 1;
	}else if(type==9){
		if(x==0)return 0;
		else if(state[y][(x-1)])return 0;
		else return 1;
	}else if(type==10){
		if(x==0)return 0;
		else if(state[y][x]||state[(y+1)][x]||state[(y+2)][x]||state[(y+3)][x])return 0;
		else return 1;
	}else if(type==11){
		if(x==0)return 0;
		else if(state[y][x]||state[(y+1)][x])return 0;
		return 1;
	}else if(type==12){
		if(x<=1)return 0;
		else if(state[y][(x-1)]||state[(y+1)][(x-2)]||state[(y+2)][(x+2)])return 0;
		else return 1;
	}else if(type==13){
		if(x==0)return 0;
		else if(state[y][(x-1)]||state[(y=1)][x])return 0;
		else return 1;
	}else if(type==14){
		if(x==0)return 0;
		else if(state[y][(x-1)]||state[(y+1)][(x-1)]||state[(y+2)][x])return 0;
		else return 1;
	}else if(type==15){
		if(x<=1)return 0;
		else if(state[y][(x-1)]||state[(y+1)][(x-2)])return 0;
		else return 1;
	}else;
	return 0;
}
int can_right(int x,int y,int type){
	if(type==1){
		if(x>=9)return 0;
		else if(state[y][(x+1)]||state[(y+1)][(x+1)])return 0;
		else return 1;
	}else if(type==2){
		if(x>=8)return 0;
		else if(state[y][(x+1)]||state[(y+1)][(x+1)]||state[(y+2)][(x+2)])return 0;
		else return 1;
	}else if(type==3){
		if(x>=7)return 0;
		else if(state[y][(x+3)]||state[(y+1)][(x+1)])return 0;
		else return 1;
	}else if(type==4){
		if(x>=8)return 0;
		else if(state[y][(x+2)]||state[(y+1)][(x+2)]||state[(y+2)][(x+2)])return 0;
		else return 1;
	}else if(type==5){
		if(x>=7)return 0;
		else if(state[y][(x+1)]||state[(y+1)][(x+3)])return 0;
		else return 1;
	}else if(type==6){
		if(x>=8)return 0;
		else if(state[y][(x+2)]||state[(y+1)][(x+1)]||state[(y+2)][(x+1)])return 0;
		else return 1;
	}else if(type==7){
		if(x>=7)return 0;
		else if(state[y][(x+3)]||state[(y+1)][(x+3)])return 0;
		else return 1;
	}else if(type==8){
		if(x>=9)return 0;
		else if(state[y][(x+1)]||state[(y+1)][(x+1)]||state[(y+2)][(x+1)])return 0;
		else return 1;
	}else if(type==9){
		if(x>=6)return 0;
		else if(state[y][(x+4)])return 0;
		else return 1;
	}else if(type==10){
		if(x>=9)return 0;
		else if(state[y][(x+1)]||state[(y+1)][(x+1)]||state[(y+2)][(x+1)]||state[(y+3)][(x+1)])return 0;
		else return 1;
	}else if(type==11){
		if(x>=8)return 0;
		else if(state[y][(x+2)]||state[(y+1)][(x+2)])return 0;
		else return 1;
	}else if(type==12){
		if(x>=9)return 0;
		else if(state[y][(x+1)]||state[(y+1)][(x+1)]||state[(y+2)][x])return 0;
		else return 1;
	}else if(type==13){
		if(x>=7)return 0;
		else if(state[y][(x+2)]||state[(y+1)][(x+3)])return 0;
		else return 1;
	}else if(type==14){
		if(x>=8)return 0;
		else if(state[y][(x+1)]||state[(y+1)][(x+2)]||state[(y+2)][(x+2)])return 0;
		else return 1;
	}else if(type==15){
		if(x>=8)return 0;
		else if(state[y][(x+2)]||state[(x+1)][(y+1)])return 0;
		else return 1;
	}else;

	return 0;
}
int can_down(int x,int y,int type){
	if(type==1){
		if(y>=8)return 0;
		else if(state[(y+2)][x]||state[(y+2)][(x-1)]||state[(y+2)][(x-2)])return 0;
		else return 1;
	}else if(type==2){
		if(y>=7)return 0;
		else if(state[(y+3)][x]||state[(y+3)][(x+1)])return 0;
		else return 1;
	}else if(type==3){
		if(y>=8)return 0;
		else if(state[(y+2)][x]||state[(y+1)][(x+1)]||state[(y+1)][(x+2)])return 0;
		else return 1;
	}else if(type==4){
		if(y>=7)return 0;
		else if(state[(y+1)][x]||state[(y+3)][(x+1)])return 0;
		else return 1;
	}else if(type==5){
		if(y>=8)return 0;
		else if(state[(y+2)][x]||state[(y+2)][(x+1)]||state[(y+2)][(x+2)])return 0;
		else return 1;
	}else if(type==6){
		if(y>=7)return 0;
		else if(state[(y+1)][(x+1)]||state[(y+3)][x])return 0;
		else return 1;
	}else if(type==7){
		if(y>=8)return 0;
		else if(state[(y+1)][x]||state[(y+1)][(x+1)]||state[(y+2)][(x+2)])return 0;
		else return 1;
	}else if(type==8){
		if(y>=7)return 0;
		else if(state[(y+3)][x]||state[(y+3)][(x-1)])return 0;
		else return 1;
	}else if(type==9){
		if(y>=9)return 0;
		else if(state[(y+1)][x]||state[(y+1)][(x+1)]||state[(y+1)][(x+2)])return 0;
		else return 1;
	}else if(type==10){
		if(y>=6)return 0;
		else if(state[(y+4)][x])return 0;
		else return 1;
	}else if(type==11){
		if(y>=8)return 0;
		else if(state[(y+2)][x]||state[(y+2)][(x+1)])return 0;
		else return 1;
	}else if(type==12){
		if(y>=7)return 0;
		else if(state[(y+2)][x]||state[(y+3)][(x-1)])return 0;
		else return 1;
	}else if(type==13){
		if(y>=8)return 0;
		else if(state[(y+1)][x]||state[(y+2)][(x+1)]||state[(y+2)][(x+2)])return 0;
		else return 1;
	}else if(type==14){
		if(y>=7)return 0;
		else if(state[(y+2)][x]||state[(y+3)][(x+1)])return 0;
		else return 1;
	}else if(type==15){
		if(y>=8)return 0;
		else if(state[(y+1)][(x+1)]||state[(y+2)][x]||state[(y+2)][(x-1)])return 0;
		else return 1;
	}else;


	return 1;
}
