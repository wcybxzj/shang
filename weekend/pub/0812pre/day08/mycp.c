#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main(int num, char *arr[]) 
{
	int rd, wd;
	char buf[10];
	int cnt, ret, pos;

	if (num < 3) {
		return -1;
	}
	rd = open(arr[1], O_RDONLY);
	if (rd < 0) {
		return -1;
	}
	wd = open(arr[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (wd < 0) {
		return -1;
	}

	while (1) {
		cnt = read(rd, buf, 10);
		if (cnt == 0) {
			break;
		}
		pos = 0;
		while (cnt > 0) {
			ret = write(wd, buf + pos, cnt);
			cnt -= ret;
			pos += ret;
		}
	}	

	close(rd);
	close(wd);

	return 0;
}
