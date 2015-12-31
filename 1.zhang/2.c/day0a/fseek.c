#include <stdio.h>

int main(void)
{
	FILE *fp = NULL;
	char buf[32] = {};
	int cnt;

	fp = fopen("txt", "r");
	//if error

	fseek(fp, 5, SEEK_SET);		

	fseek(fp, -9, SEEK_END);
		
	rewind(fp);

	cnt = fread(buf, 1, 32, fp);	

	fwrite(buf, 1, cnt, stdout);
	
	fclose(fp);

	return 0;
}
