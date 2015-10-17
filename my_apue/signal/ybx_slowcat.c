#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#define BUFSIZE 9

//extern __sighandler_t signal (int __sig, __sighandler_t __handler);
//typedef void (*sighandler_t)(int);

int loop = 0;
static void ybxsig_handler(int n){
	loop++;
	alarm(1);
}

int main(int argc, const char *argv[])
{
	char buf[BUFSIZE];
	int fp1, fp2=1;
	int ret, len;

	do{
		fp1 = open(argv[1], O_RDONLY);
		if (fp1 < 0) {
			if (errno != EINTR) {
				perror("open");
				exit(1);
			}
		}
	}while(fp1<0);

	signal(SIGALRM, ybxsig_handler);
	alarm(1);

	while (1) {
		if (loop == 0) {
			continue;
		}
		loop--;

		len = read(fp1, buf, BUFSIZE);
		if (len == 0) {
			break;//EOF
		}

		if (len < 0) {
			if (errno == EINTR) {
				continue;
			}
			perror("read");
			printf("len is %d\n", len);
			break;
		}

		ret = write(fp2, buf, len);
		if (ret < 0) {
			if (errno == EINTR) {
				continue;
			}
			perror("write()");
			break;
		}
	}
	close(fp1);

	return 0;
}
