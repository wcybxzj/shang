#include <stdio.h>
#include <time.h>

int main(void)
{
	time_t tim;
	struct tm *p = NULL;

	time(&tim);	
	
	p = localtime(&tim);

	fprintf(stdout, "%d年%d月%d日 %02d:%02d:%02d\n",\
			p->tm_year+1900, p->tm_mon+1,\
			p->tm_mday, p->tm_hour, p->tm_min,\
			p->tm_sec);

	return 0;
}
