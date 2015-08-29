#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void)
{
	int fd;
	int cnt;
	char buf[32] = {};

	fd = open("./open.c", O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "open() error\n");
		return -1;
	}	

	while (1) {
		cnt = read(fd, buf, 5);
		if (cnt == 0) {
			break;
		}
		write(1, buf, cnt);
	}

	close(fd);

	return 0;
}

