#include <stdio.h>
#define LEN 8
/*
 * 91/2 = 45 1
 * 45/2 = 22 1
 * 22/2 = 11 0
 * 11/2 = 5  1
 * 5/2  = 2  1
 * 2/2  = 1  0
 * 1/2  = 0  1
 */

/*
 * 8/2 = 4 0
 * 4/2 = 2 0
 * 2/2 = 1 0
 * 1/2 = 0 1
 */

int main(int argc, const char *argv[])
{
	int num, tmp_num;
	int jinzhi;
	int left_num;
	scanf("%d %d", &num, &jinzhi);
	char result[LEN]={};//初始化为0
	int i = LEN-1;

	while (1) {
		if (i < 0) {
			printf("the arr's len is only 8\n");
			break;
		}

		tmp_num = (num/jinzhi);
		printf("===========\n");
		printf("%d/%d=%d\n",num, jinzhi, tmp_num);

		printf("i is %d\n", i);
		result[i] = num%jinzhi;
		printf("%d %c %d=%d\n", num, '%' ,jinzhi, result[i]);

		i--;
		num = tmp_num;
		//printf("num is %d\n", num);
		if(tmp_num==0) {
			break;
		}

	}

	for (i = 0; i < LEN; i++) {
		printf("%d ", result[i]);
	}

	return 0;
}
