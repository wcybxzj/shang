#include <stdio.h>
int main(int argc, const char *argv[])
{
	int c;
	int c_num = 0;
	int w_num = 0;
	int r_num = 0;
	int flag = 0;
	//ctrl+D 终结
	while ((c = getchar()) != EOF) {
		c_num++;
		if (c=='\n') {
			r_num++;
		}
		if (c != '\n' && c != ' ') {
			flag++;
			if (flag == 1) {
				w_num++;
			}
		}else{
			flag = 0;
		}
	}
	printf("c%d w%d r%d\n",c_num, w_num, r_num);
	return 0;
}
