#include "../include/ini.h"
extern int state[10][9];
extern int pos[10][9][2];
void box(unsigned short *buf,int x,int y,unsigned short color){
	int i,j;
	for(j=pos[y][x][1];j<pos[y][x][1]+WH;j++)
		for(i=pos[y][x][0];i<pos[y][x][0]+WH;i++)
				if((j==pos[y][x][1])||(j==pos[y][x][1]+WH-1)||(i==pos[y][x][0])||(i==pos[y][x][0]+WH-1))
					buf[j*SCREEN_X+i]=0x7e0;
				else 	buf[j*SCREEN_X+i]=color;
	if(color==BGCOLOR)state[y][x]=0;
	else if(color==FOCOLOR)state[y][x]=1;
	else ;
	return;
}
void paint(unsigned short *buf_fb,int x,int y,int type,unsigned short color){
	if(type==1){
		box(buf_fb,x,y,color);
		box(buf_fb,x,(y+1),color);
		box(buf_fb,(x-1),(y+1),color);
		box(buf_fb,(x-2),(y+1),color);
		if(color==BGCOLOR){//这一步在box函数中已经做了。
		}else if(color==FOCOLOR){
		}else;
	}else if(type==2){
		box(buf_fb,x,y,color);
		box(buf_fb,x,(y+1),color);
		box(buf_fb,x,(y+2),color);
		box(buf_fb,(x+1),(y+2),color);
	}else if(type==3){
		box(buf_fb,x,y,color);
		box(buf_fb,(x+1),y,color);
		box(buf_fb,(x+2),y,color);
		box(buf_fb,x,(y+1),color);
	}else if(type==4){
		box(buf_fb,x,y,color);
		box(buf_fb,(x+1),y,color);
		box(buf_fb,(x+1),(y+1),color);
		box(buf_fb,(x+1),(y+2),color);
	}else if(type==5){
		box(buf_fb,x,y,color);
		box(buf_fb,x,(y+1),color);
		box(buf_fb,(x+1),(y+1),color);
		box(buf_fb,(x+2),(y+1),color);
	}else if(type==6){
		box(buf_fb,x,y,color);
		box(buf_fb,(x+1),y,color);
		box(buf_fb,x,(y+1),color);
		box(buf_fb,x,(y+2),color);
	}else if(type==7){
		box(buf_fb,x,y,color);
		box(buf_fb,(x+1),y,color);
		box(buf_fb,(x+2),y,color);
		box(buf_fb,(x+2),(y+1),color);
	}else if(type==8){
		box(buf_fb,x,y,color);
		box(buf_fb,x,(y+1),color);
		box(buf_fb,x,(y+2),color);
		box(buf_fb,(x-1),(y+2),color);
	}else if(type==9){
		box(buf_fb,x,y,color);
		box(buf_fb,(x+1),y,color);
		box(buf_fb,(x+2),y,color);
		box(buf_fb,(x+3),y,color);
	}else if(type==10){
		box(buf_fb,x,y,color);
		box(buf_fb,x,(y+1),color);
		box(buf_fb,x,(y+2),color);
		box(buf_fb,x,(y+3),color);
	}else if(type==11){
		box(buf_fb,x,y,color);
		box(buf_fb,(x+1),y,color);
		box(buf_fb,x,(y+1),color);
		box(buf_fb,(x+1),(y+1),color);
	}else if(type==12){
		box(buf_fb,x,y,color);
		box(buf_fb,x,(y+1),color);
		box(buf_fb,(x-1),(y+1),color);
		box(buf_fb,(x-1),(y+2),color);
	}else if(type==13){
		box(buf_fb,x,y,color);
		box(buf_fb,(x+1),y,color);
		box(buf_fb,(x+1),(y+1),color);
		box(buf_fb,(x+2),(y+1),color);
	}else if(type==14){
		box(buf_fb,x,y,color);
		box(buf_fb,x,(y+1),color);
		box(buf_fb,(x+1),(y+1),color);
		box(buf_fb,(x+1),(y+2),color);
	}else if(type==15){
		box(buf_fb,x,y,color);
		box(buf_fb,(x+1),y,color);
		box(buf_fb,x,(y+1),color);
		box(buf_fb,(x-1),(y+1),color);
	}else ;
	return;
}
