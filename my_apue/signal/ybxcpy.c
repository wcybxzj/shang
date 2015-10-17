#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define BUFSIZE 1024

int main(int argc, const char *argv[])
{
	char buf[BUFSIZE];
	int fp1, fp2;
	int ret, len;
	fp1 = open(argv[1], O_RDONLY);
	fp2 = open(argv[2], O_WRONLY|O_TRUNC|O_CREAT);

	while (1) {
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
	close(fp2);

	return 0;
}
