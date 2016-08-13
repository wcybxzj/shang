#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define TTY1 "/dev/tty11"
#define TTY2 "/dev/tty12"

#define BUFSIZE 1024

enum{
	STATE_R=1,
	STATE_W,
	STATE_Ex,
	STATE_T
};

struct fsm_st{
	int state;
	int sfd;
	int dfd;
	char buf[BUFSIZE];
	int len;
	int pos;
	char *errstr;
};

static void fsm_driver(struct fsm_st *fsm) {
	int ret;
	switch(fsm->state){
		case STATE_R:
			fsm->len = read(fsm->sfd, fsm->buf, BUFSIZE);
			//printf("%d\n",fsm->len);
			//sleep(1);
			if(fsm->len == 0){
				fsm->state = STATE_T;
			}else if(fsm->len < 0){
				if(errno == EAGAIN || errno == EINTR){
					fsm->state = STATE_R;
				}else{
					fsm->state = STATE_Ex;
					fsm->errstr = "read():";
				}
			}else{
				fsm->pos = 0;
				fsm->state = STATE_W;
			}
			break;
		case STATE_W:
			ret = write(fsm->dfd,fsm->buf+fsm->pos, fsm->len);
			if (ret<0) {
				if(errno == EAGAIN || errno == EINTR){
					fsm->state = STATE_W;
				}else{
					fsm->errstr="write():";
					fsm->state = STATE_Ex;
				}
			}else if(ret>0){
				fsm->len -= ret;
				fsm->pos += ret;
				if(fsm->len == 0){
					fsm->state = STATE_R;
				}else{
					fsm->state = STATE_W;
				}
			}else{
				fsm->state = STATE_T;
			}
			break;
		case STATE_Ex:
			perror(fsm->errstr);
			fsm->state = STATE_T;
			break;
		case STATE_T:
			//exit(1);
			break;
		default:
			break;
	}
}

static void relay(int fd1, int fd2){
	struct fsm_st fs12, fs21;
	int fd1_save, fd2_save;

	fd1_save = fcntl(fd1, F_GETFL);
	fd2_save = fcntl(fd2, F_GETFL);
	fcntl(fd1, fd1_save|O_NONBLOCK);
	fcntl(fd2, fd2_save|O_NONBLOCK);

	fs12.state = STATE_R;
	fs12.sfd = fd1;
	fs12.dfd = fd2;

	fs21.state = STATE_R;
	fs21.sfd = fd2;
	fs21.dfd = fd1;

	while(fs12.state!=STATE_T || fs21.state!=STATE_T){
		//sleep(1);
		fsm_driver(&fs12);
		fsm_driver(&fs21);
	}

	fcntl(fd1, F_SETFL, fd1_save);
	fcntl(fd2, F_SETFL, fd2_save);

}

int main(){
	int fd1, fd2;
	fd1 = open(TTY1, O_RDWR|O_NONBLOCK);
	if(fd1 < 0){
		perror("open()");	
		exit(1);
	}
	write(fd1, "MYTTY1\n", 7);	

	fd2 = open(TTY2, O_RDWR|O_NONBLOCK);
	if(fd2 < 0){
		perror("open()");	
		exit(1);
	}
	write(fd2, "MYTTY2\n", 7);	

	relay(fd1, fd2);

	close(fd1);
	close(fd2);

	exit(0);
}
