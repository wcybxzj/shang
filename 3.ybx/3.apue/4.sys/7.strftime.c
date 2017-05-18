#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *get_date_str();

int main() {
	char *filename;
	filename = get_date_str();
	printf("%s\n", filename);//2017-05-18 16-35-30

	strcat(filename, ".bmp");
	printf("%s\n", filename);//2017-05-18 16-37-01.bmp
	return 0;
}

char *get_date_str(){
	time_t mytime;
	struct tm *mytm;
	char* str=malloc(50);
	mytime = time(NULL);
	mytm = localtime(&mytime);
	strftime(str, 50, "%Y-%m-%d %H-%M-%S", mytm);
	return str;
}
