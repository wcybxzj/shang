#include <aio.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <strings.h>

#define FNAME "1.txt"
#define BUF_SIZE 4096
#define NBUF 10
void aio_completion_handler( int signo, siginfo_t *info, void *context )
{
	int ret;
	struct aiocb *req;
	if (info->si_signo == SIGIO) {
		req = (struct aiocb *)info->si_value.sival_ptr;
		ret =  aio_return(req);
		//printf("ret:%d\n", ret);
		if (ret > 0) {
			printf("%s", req->aio_buf);
			fflush(NULL);
			//printf("ret:%d\n", ret);
		} else {
			perror("aio_return");
		}
	}
	return;
}

void setup_io(int fd)
{
	int ret;
	struct sigaction sig_act;
	struct aiocb my_aiocb;
	off_t off = 0;
	struct stat sbuf;
	const struct aiocb *aiolist[NBUF];

	if (fstat(fd, &sbuf)<0) {
		perror("fstat");
		exit(1);
	}

	/* Set up the signal handler */
	sigemptyset(&sig_act.sa_mask);
	sig_act.sa_flags = SA_SIGINFO;
	sig_act.sa_sigaction = aio_completion_handler;

	/* Set up the AIO request */
	bzero( (char *)&my_aiocb, sizeof(struct aiocb) );
	my_aiocb.aio_fildes = fd;
	my_aiocb.aio_buf = malloc(BUF_SIZE+1);
	my_aiocb.aio_nbytes = BUF_SIZE;

	/* Link the AIO request with the Signal Handler */
	my_aiocb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
	my_aiocb.aio_sigevent.sigev_signo = SIGIO;
	my_aiocb.aio_sigevent.sigev_value.sival_ptr = &my_aiocb;

	/* Map the Signal to the Signal Handler */
	ret = sigaction( SIGIO, &sig_act, NULL );
	if (ret) {
		perror("sigaction");
		exit(1);
	}

	while (off < sbuf.st_size) {
		bzero((char *)my_aiocb.aio_buf, BUF_SIZE+1); 
		my_aiocb.aio_offset = off;
		off+=BUF_SIZE;
		ret = aio_read( &my_aiocb );
		if (ret < 0) {
			perror("aio_read");
			exit(1);
		}

		//业务代码
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);
		//printf("===========off:%d============\n",off);

		//方法1:
		//ret =  aio_error(&my_aiocb);
		//if(ret == EINPROGRESS ){
		//	sleep(1);
		//}

		//方法2:
		//printf("suspend\n");
		if (aio_suspend(aiolist, NBUF, NULL) < 0){
			printf("aio_suspend failed");
			exit(1);
		}
		//sleep(1);

	}
	return;
}

int main(int argc, const char *argv[])
{
	int fd, ret;
	struct aiocb my_aiocb;

	fd = open( FNAME, O_RDONLY );
	if (fd < 0) perror("open");

	setup_io(fd);

	pause();

	return 0;
}
