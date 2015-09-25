#include "../include/ini.h"

int state[10][9];//每个方格的状态
int pos[10][9][2];//每个方格的左上角的坐标
struct termios  old;
int type,next;
int x=4,y=0;
int sco;
void test(){
	int i,j;
	for(i=0;i<10;i++){
		for(j=0;j<9;j++)
			printf("%3d",state[i][j]);
		printf("\n");
	}
	return ;
}
int main(int argc,char **argv){
	int fb,ret;
	int d;
	unsigned char a;
	unsigned char buf[51];
	unsigned short *buf_fb;
	sco=0;
	fb=open("/dev/fb0",O_RDWR);
	if(fb<0){
		perror("Open fb device error\n");
		_exit(1);
	}
	buf_fb=mmap(buf,SCREEN_X*SCREEN_Y*COLOR_BIT_COUNT,PROT_READ|PROT_WRITE,MAP_SHARED,fb,0);
	if(buf_fb==NULL){
		perror("Map fb device fault\n");
		_exit(2);
	}
	init();
	system("clear");
	//----------------------------------------------------------------------------------
	set_cursor_raw(&old);
	//创建一个线程随时改变a的值
	pthread_t thread;
	pthread_create(&thread,NULL,pthread_fun,&a);
	type=make_rand();
	redraw(buf_fb);
	while(1){
		next=make_rand();
		if(!check_rnd(type)){//false:continue;
			next=make_rand();
			type=next;
			continue;
		}
		x=4;y=0;a='f';
		paint(buf_fb,x,y,type,FOCOLOR);
		sleep(1);
		while(1){
			if(a=='x'){
				if(check_can_change(type,x,y)){
					paint(buf_fb,x,y,type,BGCOLOR);//draw
					change_type();
				}
			}else if(a=='a'){//left
				if(can_left(x,y,type)){
					paint(buf_fb,x,y,type,BGCOLOR);
					x--;
				}
			}else if(a=='d'){//right
				if(can_right(x,y,type)){
					paint(buf_fb,x,y,type,BGCOLOR);
					x++;
				}
			}else if(a=='s'){//down
				if(can_down(x,y,type)){
					paint(buf_fb,x,y,type,BGCOLOR);
					y++;
				}
			}else;
			a='f';
			paint(x,y,type,FOCOLOR);//draw
			if(!can_down(x,y,type)){
				type=next;
				a='f';
				score();
				redraw(buf_fb);
				//test();
				break;
			}
			paint(buf_fb,x,y,type,BGCOLOR);//draw
			y+=1;
			paint(buf_fb,x,y,type,FOCOLOR);//draw
			sleep(1);
		}
		type=next;
	}

	tcsetattr(0,TCSANOW,&old);
	close(fb);
	return 0;
}
