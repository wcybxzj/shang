#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#define FNAME "log"
#define SIZE 128


int main(int argc, const char *argv[])
{
	FILE *fp;
	int num = 0;
	time_t mytime;
	struct tm *mytm;
	char str[SIZE];
	char tmp[SIZE];
	//int count =0;

	fp = fopen(FNAME, "a+");


	while (fgets(tmp, SIZE, fp)) {
		num++;
	}

	while (1) {
		//count++;
		//if (count >10) {
		//	break;
		//}
		mytime = time(NULL);
		//printf("%ld\n", mytime);
		mytm = localtime(&mytime);
		strftime(str, SIZE, "%Y-%m-%d %H-%M-%S", mytm);
		fprintf(fp, "%d %s\n", num, str);
		fflush(fp);
		num++;
		//printf("%s\n", str);
		sleep(1);
	}

	return 0;
}
