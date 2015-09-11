#include <stdio.h>
#include <time.h>
#include <string.h>
#define BUFFSIZE 100
int main(int argc, const char *argv[])
{
	FILE *fp;
	fp = fopen("time.txt", "r");
	if (fp==NULL) {
		fp = fopen("time.txt", "w+");
	} else {
		char buff[BUFFSIZE] ={};
		int num = fread(buff, 1, BUFFSIZE, fp);
		printf("上一次的时间是:\n");
		fwrite(buff, 1, BUFFSIZE, stdout);
	}

	time_t tim;
	struct tm *p = NULL;
	time(&tim);

	p = localtime(&tim);

	char str[BUFFSIZE] = {};
	int len = sprintf(str, "%d年%d月%d日 %02d:%02d:%02d\n",\
			p->tm_year+1900, p->tm_mon+1,\
			p->tm_mday, p->tm_hour, p->tm_min,\
			p->tm_sec);

	//printf("\n=========本次时间:=========\n");
	//printf("str len is%s\n", len);
	//fwrite(str, 1, len, fp);
	fclose(fp);

	return 0;
}
