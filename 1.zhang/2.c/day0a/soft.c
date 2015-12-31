#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FLNAME "save.txt"

int main(void)
{
	FILE *fp = NULL;
	time_t tim;
	struct tm *p = NULL;
	char buf[100] = {};

	if ((fp = fopen(FLNAME, "r")) == NULL) {
		//first
		if ((fp = fopen(FLNAME, "w+")) == NULL) {
			perror("fopen()");
			exit(1);
		} else {
			time(&tim);
			p = localtime(&tim);		
			fprintf(fp, "%d年%d月%d日 %02d:%02d:%02d\n", \
					p->tm_year+1900, p->tm_mon+1, p->tm_mday, \
					p->tm_hour, p->tm_min, p->tm_sec);

			rewind(fp);
			fgets(buf, 100, fp);
			printf("%s 第一次打开的时间为\n", FLNAME);
			fputs(buf, stdout);
		}
	}else {
		//multi
		rewind(fp);
		printf("%s 上一次打开的时间为\n", FLNAME);		
		fgets(buf, 100, fp);
		fputs(buf, stdout);

		fclose(fp);

		fp = fopen(FLNAME, "w+");
		if (fp == NULL)
			exit(1);
		time(&tim);
		p = localtime(&tim);
		fprintf(fp, "%d年%d月%d日 %02d:%02d:%02d\n", \
				p->tm_year+1900, p->tm_mon+1, p->tm_mday, \
				p->tm_hour, p->tm_min, p->tm_sec);

		rewind(fp);

		printf("%s 本次打开的时间为\n", FLNAME);
		fgets(buf, 100, fp);
		fputs(buf, stdout);
	}	

	fclose(fp);

	exit(0);
}
