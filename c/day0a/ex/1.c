#include <stdio.h>
#include <string.h>
void func1()
{
	FILE *fp = fopen("1.txt", "w+");
	int ch;
	char *str = "中国";
	int num = fwrite(str, 1, strlen(str), fp);
	printf("%d\n", num);


	//while ((ch = fgetc(fp)) !=EOF) {
	//	fputc(ch, stdout);
	//	fflush(stdout);
	//	sleep(1);
	//}

}

void func2()
{
	FILE *fp = fopen("1.txt", "r");
	int ch;
	while ((ch = fgetc(fp)) !=EOF) {
		fputc(ch, stdout);
		fflush(stdout);
		sleep(1);
	}
}



int main(int argc, const char *argv[])
{
	func1();
	return 0;
}
