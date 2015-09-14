#include <stdio.h>
#include <stdlib.h>

#define FLNAME "fgets.c"
#define BUFSIZE 20

#define EXIT(name)\
	do {\
	fprintf(stderr, "%s: %s[%d] %s is error\n", \
			__FILE__, __FUNCTION__, __LINE__, #name);\
	exit(EXIT_FAILURE);\
	}while (0)

int main(void)
{
	FILE *fp = NULL;
	char buf[BUFSIZE];
	int cnt;

#if 1
	fp = fopen(FLNAME, "r");
	if (NULL == fp) 
		EXIT(open());
	
	while (1) {
		cnt = fread(buf, 1, BUFSIZE, fp);	
		if (cnt <= 0)
			break;
		fwrite(buf, 1, cnt, stdout);
	}	
	
	fclose(fp);
#endif
	return 0;
}
