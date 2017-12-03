#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define TTY1            "/dev/tty11"
#define TTY2            "/dev/tty12"

#define BUFSIZE         1024

enum
{
	STATE_R=1,
	STATE_W,
	STATE_Ex,
	STATE_T
};

struct fsm_st
{
	int state;
	int sfd;
	int dfd;
	char buf[BUFSIZE];
	int len;
	int pos;
	char *errstr;
}; 

static void fsm_driver(struct fsm_st *fsm){
	int ret;
	switch(fsm->state) 
	{ 
		case STATE_R:
			fsm->len = read(fsm->sfd,fsm->buf,BUFSIZE);
			if(fsm->len == 0)
				fsm->state = STATE_T;
			else if(fsm->len < 0)
			{
				if(errno == EAGAIN)
					fsm->state = STATE_R;
				else
				{
					fsm->errstr = "read()";
					fsm->state = STATE_Ex;
				}
			}
			else // len > 0
			{
				fsm->pos = 0;
				fsm->state = STATE_W;
			}
			break;

		case STATE_W:
			ret = write(fsm->dfd,fsm->buf+fsm->pos,fsm->len);
			if(ret < 0)
			{
				if(errno == EAGAIN)
					fsm->state = STATE_W;
				else
				{
					fsm->errstr = "write()";
					fsm->state = STATE_Ex;
				}
			}
			else
			{
				fsm->pos += ret;
				fsm->len -= ret;
				if(fsm->len == 0)
					fsm->state = STATE_R;
				else
					fsm->state = STATE_W;
			}

			break;

		case STATE_Ex:
			perror(fsm->errstr);
			fsm->state = STATE_T;
			break;

		case STATE_T:
			/*exit(0),do sth*/
			break;

		default:
			abort();
	}

}

static void rely(int sfd, int dfd){
	int fd;
	struct fsm_st fs;

	fs.state = STATE_R;
	fs.sfd = sfd;
	fs.dfd = dfd;
	while(1){
		fsm_driver(&fs);
	}	
}

//虚拟机进入/dev/tty11 ctrl+Alt+f11
//回去ctrl_alt+f1
int main(void){
	int i;
	int fd1, fd2;
	fd1 = open(TTY1, O_RDWR);
	fd2 = open(TTY2, O_RDWR);

	pid_t pid;
	pid = fork();	
	if(pid>0){
		rely(fd1, fd2);
	}else{
		rely(fd2, fd1);
	}
	for(i=0 ;i<2;i++){
		wait(NULL);
	}
}
