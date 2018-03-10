#include <stdio.h>
#include <string.h>
//#define NAME "appendonly.aof"
#define NAME "123.txt"

int main(int argc, const char *argv[])
{
	FILE *fp = fopen(NAME,"r");
	char buf[128];
	if (fgets(buf,sizeof(buf),fp) == NULL) {
	
	}

	printf("%s\n",buf);
	printf("%d\n",strlen(buf));
	int len = strlen(buf);
	int i;
	for (i = 0; i < len; i++) {
		printf("%x\n",buf[i]);
	}

	return 0;
}
