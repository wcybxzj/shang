#include <stdio.h>
#include <string.h>

typedef enum {DEC_BIN = 1, DEC_HEX}chose_t;

int main(void)
{
	chose_t choose;
	int num;
	char arr[8] = {};
	int i;

	while (1) {
		system("clear");
		printf("按1:十进制数转换成二进制\n按2.十进制转换成十六进制\n");
		scanf("%d", &choose);
		printf("输入您要转换的数：");
		scanf("%d", &num);
		if (num < 0)
			break;

		switch (choose) {
			case DEC_BIN:
				memset(arr, 0x00, sizeof(arr));
				i = 0;
				while (num) {
					arr[7-i] = num % 2;
					num /= 2;
					i++;
				}	
				break;	
			case DEC_HEX:
				memset(arr, 0x00, sizeof(arr));
				for (i = 7; i>= 0 && num; i--) {
					arr[i] = num % 16;	
					num /= 16;
				}
				break;
			default:
				break;
		}
		printf("the result is :\n");
		for (i = 0; i < 8; i++) {
			if (arr[i] > 9) {
				printf("%c", ('a'+arr[i]-10));
			} else {
				printf("%d", arr[i]);
			}
		}
		printf("\n");
		sleep(2);
	}

	return 0;
}
