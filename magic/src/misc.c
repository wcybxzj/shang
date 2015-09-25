#include "../include/ini.h"

extern int state[10][9];
extern int pos[10][9][2];
extern int type;

void init(){
	int i,j;
	int x,y;
	x=BEGIN_X;
	y=BEGIN_Y;

	for(i=0;i<10;i++)
		for(j=0;j<9;j++)
			state[i][j]=0;
	
	for(i=0;i<10;i++){
		x=BEGIN_X;
		for(j=0;j<9;j++){
			pos[i][j][0]=x;
			pos[i][j][1]=y;
			x+=WH;
		}
		y+=WH;	
	}
	return;
}
int check_can_change(int type,int x,int y){
	if(type==1){
		if((y==0)||(x==8))return 0;
		else if(state[(y-1)][x]||state[(y+1)][(x+1)])return 0;
		else return 1;
	}else if(type==2){
		if(x>6)return 0;
		else if(state[y][(x+1)]||state[y][(x+2)])return 0;
		else return 1;
	}else if(type==3){
		if(y>7)return 0;
		else if(state[(y+1)][(x+2)]||state[(y+2)][(x+2)])return 0;
		else return 1;
	}else if(type==4){
		if(x==0)return 0;
		else if(state[(y+1)][x]||state[(y+1)][(x-1)])return 0;
		else return 1;
//-------------------------------------------------------------------------------------
	}else if(type==5){
		if(y>7)return 0;
		else if(state[(y+2)][x]||state[y][(x+1)])return 0;
		else return 1;
	}else if(type==6){
		if(x>6)return 0;
		else if(state[y][(x+2)]||state[(y+1)][(x+2)])return 0;
		else return 1;
	}else if(type==7){
		if(y==0)return 0;
		else if(state[(y+1)][(x+3)]||state[y][(x+3)]||state[(y-1)][(x+3)])return 0;
		else return 1;
	}else if(type==8){
		if(x>6)return 0;
		else if(state[(y+2)][(x+1)]||state[(y+2)][(x+2)])return 0;
		else return 1;
//-------------------------------------------------------------------------------------
	}else if(type==9){
		if(y>6)return 0;
		else if(state[(y+1)][x]||state[(y+2)][x]||state[(y+3)][x])return 0;
		else return 1;
	}else if(type==10){
		if(x>5)return 0;
		else if(state[y][(x+1)]||state[y][(x+2)]||state[y][(x+3)])return 0;
		else return 1;
//-------------------------------------------------------------------------------------
	}else if(type==11){
		return 1;
//-------------------------------------------------------------------------------------
	}else if(type==12){
		if(x>6)return 0;
		else if(state[(y+1)][(x+1)]||state[(y+1)][(x+2)])return 0;
		else return 1;
	}else if(type==13){
		if(y==0)return 0;
		else if(state[(y-1)][(x+1)]||state[(y+1)][x])return 0;
		else return 1;
//-------------------------------------------------------------------------------------
	}else if(type==14){
		if(x>6)return 0;
		else if(state[y][(x+1)]||state[y][(x+2)])return 0;
		else return 1;
	}else if(type==15){
		if(y>7)return 0;
		else if(state[(y+1)][(x+1)]||state[(y+2)][(x+1)])return 0;
		else return 1;
	}else return 0;
	return 0;//means not
}




int make_rand(){
	long rnd;
	srandom(random());
	rnd=random();
	return rnd%15+1;
}
//--------------------------------------------------------------------------------
void redraw(unsigned short *buf_fb){
	int i,j;
	for(i=0;i<10;i++)
		for(j=0;j<9;j++){
			if(state[i][j])box(buf_fb,j,i,FOCOLOR);
			else box(buf_fb,j,i,BGCOLOR);
		}


	return;
}
int check_rnd(int type){
	if(type==1){
		if(state[0][4])return 0;
		if(state[1][4])return 0;
		if(state[1][3])return 0;
		if(state[1][2])return 0;
		return 1;
	}
	if(type==2){
		if(state[0][4])return 0;
		if(state[1][4])return 0;
		if(state[2][4])return 0;
		if(state[2][5])return 0;
		return 1;
	}
	if(type==3){
		if(state[0][4])return 0;
		if(state[0][5])return 0;
		if(state[0][6])return 0;
		if(state[1][4])return 0;
		return 1;
	}
	if(type==4){
		if(state[0][4])return 0;
		if(state[0][5])return 0;
		if(state[1][5])return 0;
		if(state[2][5])return 0;
		return 1;
	}
	if(type==5){
		if(state[0][4])return 0;
		if(state[1][4])return 0;
		if(state[1][5])return 0;
		if(state[1][6])return 0;
		return 1;
	}
	if(type==6){
		if(state[0][4])return 0;
		if(state[0][5])return 0;
		if(state[1][4])return 0;
		if(state[2][4])return 0;
		return 1;
	}
	if(type==7){
		if(state[0][4])return 0;
		if(state[0][5])return 0;
		if(state[0][6])return 0;
		if(state[1][6])return 0;
		return 1;
	}
	if(type==8){
		if(state[0][4])return 0;
		if(state[1][4])return 0;
		if(state[2][4])return 0;
		if(state[2][3])return 0;
		return 1;
	}
	if(type==9){
		if(state[0][4])return 0;
		if(state[0][5])return 0;
		if(state[0][6])return 0;
		if(state[0][7])return 0;
		return 1;
	}
	if(type==10){
		if(state[0][4])return 0;
		if(state[1][4])return 0;
		if(state[2][4])return 0;
		if(state[3][4])return 0;
		return 1;
	}
	if(type==11){
		if(state[0][4])return 0;
		if(state[0][5])return 0;
		if(state[1][4])return 0;
		if(state[1][5])return 0;
		return 1;
	}
	if(type==12){
		if(state[0][4])return 0;
		if(state[1][4])return 0;
		if(state[1][3])return 0;
		if(state[2][3])return 0;
		return 1;
	}
	if(type==13){
		if(state[0][4])return 0;
		if(state[0][5])return 0;
		if(state[1][5])return 0;
		if(state[1][6])return 0;
		return 1;
	}
	if(type==14){
		if(state[0][4])return 0;
		if(state[1][4])return 0;
		if(state[1][5])return 0;
		if(state[2][5])return 0;
		return 1;
	}
	if(type==15){
		if(state[0][4])return 0;
		if(state[0][5])return 0;
		if(state[1][4])return 0;
		if(state[1][3])return 0;
		return 1;
	}
	return 0;
}
void change_type(){
	if(type==4)type=1;
	else if(type==8)type=5;
	else if(type==10)type=9;
	else if(type==11)type=11;
	else if(type==13)type=12;
	else if(type==15)type=14;
	else type+=1;
}
