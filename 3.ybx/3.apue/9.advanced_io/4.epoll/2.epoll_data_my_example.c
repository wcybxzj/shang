#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/epoll.h>
#include <string.h>
#define	TTY1		"/dev/tty11"
#define	TTY2		"/dev/tty12"
#define BUFSIZE		1024
enum
{
	STATE_R=1,
	STATE_W,
STATE_AUTO,
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

typedef struct fd_st{
	int fd;
	char *desc;
} FD_ST;

static void fsm_driver(struct fsm_st *fsm, struct epoll_event *ev)
{
	int ret;
	char name[100]={'\0'};
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
			fsm->len += strlen(((FD_ST *)ev->data.ptr)->desc);
			fsm->len += 2;
			snprintf(name, fsm->len, "%s:%s", ((FD_ST*)ev->data.ptr)->desc, fsm->buf+fsm->pos);
			ret = write(fsm->dfd, name, strlen(name));
			//ret = write(fsm->dfd,fsm->buf+fsm->pos,fsm->len);
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
				//fsm->pos += ret;
				//fsm->len -= ret;
				//if(fsm->len == 0)
				//	fsm->state = STATE_R;
				//else 
				//	fsm->state = STATE_W;

				fsm->state = STATE_R;
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
static int max(int a,int b)
{
	if(a > b)
		return a;
	return b;
}
static void relay(FD_ST* p1 , FD_ST* p2)
{
	int num;
	struct fsm_st fsm12,fsm21;
	int epfd;
	struct epoll_event ev;
	epfd = epoll_create(1);//传参正整数即可,无意义
	if (epfd < 0) {
		perror("epoll_create():");
		exit(1);
	}
	fsm12.state = STATE_R;
	fsm12.sfd = p1->fd;
	fsm12.dfd = p2->fd;
	fsm21.state = STATE_R;
	fsm21.sfd = p2->fd;
	fsm21.dfd = p1->fd;
	ev.events = 0;
	ev.data.ptr = p1;
	epoll_ctl(epfd, EPOLL_CTL_ADD, p1->fd, &ev);
	ev.events = 0;
	ev.data.ptr = p2;
	epoll_ctl(epfd, EPOLL_CTL_ADD, p2->fd, &ev);
	while(fsm12.state != STATE_T || fsm21.state != STATE_T)
	{
		/*布置监视任务*/
		printf("布置监视任务\n");
		ev.data.ptr = p1;
		ev.events = 0;
		if(fsm12.state == STATE_R){
			printf("fsm12 state_r\n");
			ev.events |= EPOLLIN;
		}
		if(fsm21.state == STATE_W){
			printf("fsm21 state_w\n");
			ev.events |= EPOLLOUT;
		}
		epoll_ctl(epfd, EPOLL_CTL_MOD, p1->fd, &ev);
		ev.data.ptr = p2;
		ev.events = 0;
		if(fsm12.state == STATE_W){
			printf("fsm12 state_w\n");
			ev.events |= EPOLLOUT;
		}
		if(fsm21.state == STATE_R){
			printf("fsm21 state_r\n");
			ev.events |= EPOLLIN;
		}
		epoll_ctl(epfd, EPOLL_CTL_MOD, p2->fd, &ev);
		/*监视*/
		if(fsm12.state < STATE_AUTO || fsm21.state < STATE_AUTO)
		{
			while( (num = epoll_wait(epfd, &ev, 1, -1))< 0)
			{
				if(errno == EINTR){
					continue;
				}
				perror("epoll_wait()");
				exit(1);
			}
		}
		printf("============= unblock num is %d=============\n", num);
		/*查看监视结果*/
		if( ((FD_ST *)ev.data.ptr)->fd == p1->fd && ev.events & EPOLLIN ||\
			((FD_ST *)ev.data.ptr)->fd == p2->fd && ev.events & EPOLLOUT ||\
				fsm12.state > STATE_AUTO ){
			fsm_driver(&fsm12, &ev);
		}
		if( ((FD_ST *)ev.data.ptr)->fd == p2->fd && ev.events & EPOLLIN ||\
			((FD_ST *)ev.data.ptr)->fd == p1->fd && ev.events & EPOLLOUT ||\
				fsm21.state > STATE_AUTO ){
			fsm_driver(&fsm21, &ev);
		}
	}
	close(epfd);
}

/*
typedef union epoll_data {
               void    *ptr; //epoll精华
               int      fd;  //
               uint32_t u32;
               uint64_t u64;
           } epoll_data_t;

struct epoll_event {
               uint32_t     events;
               epoll_data_t data;
           };
*/
//http://ersun.warnckes.com/code/c/server.html
int main()
{
	FD_ST* fd_arr[2]={};
	FD_ST* p1 = NULL;
	FD_ST* p2 = NULL;
	int fd1,fd2;

	fd1 = open(TTY1,O_RDWR|O_NONBLOCK);
	if(fd1 < 0){
		perror("open():");
		exit(1);
	}
	write(fd1,"TTY1\n",5);
	fd2 = open(TTY2,O_RDWR);
    if(fd2 < 0){
        perror("open():");
        exit(1);
    }
	write(fd2,"TTY2\n",5);

	p1 = malloc(sizeof(FD_ST));
	if(p1 == NULL){
		perror("malloc():");
        exit(1);
	}
	p2 = malloc(sizeof(FD_ST));
	if(p2 == NULL){
		perror("malloc():");
        exit(1);
	}

	p1->fd = fd1;
	p2->fd = fd2;
	p1->desc = "this is fd1";
	p2->desc = "this is fd2";
	fd_arr[0] = p1;
	fd_arr[1] = p2;
	relay(fd_arr[0], fd_arr[1]);
	close(fd1);
	close(fd2);
	free(p1);
	free(p2);
	exit(0);
}
