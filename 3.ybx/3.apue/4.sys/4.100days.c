#include <stdio.h>
#include <time.h>

#define SIZE 1000
int main(int argc, const char *argv[])
{
	time_t mytime;
	char str[SIZE];
	struct tm *mytm;
	mytime = time(NULL);
	mytm = localtime(&mytime);
	strftime(str, SIZE, "NOW:%Y-%m-%d %H-%M-%S", mytm);
	printf("%s\n",str);

	mytime += 86400*100;
	mytm = localtime(&mytime);
	strftime(str, SIZE, "100 days after:%Y-%m-%d %H-%M-%S", mytm);
	printf("%s\n",str);

	return 0;
}
