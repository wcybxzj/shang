#include <stdio.h>
#define NUM 10
//约瑟夫环，每数到3杀一个人 ,看最后剩下谁
int main(int argc, const char *argv[])
{
	int i;
	int arr[NUM] = {};
	int count = 0;
	int all_kill = 0;

	for (i=0; all_kill<NUM-1 || !printf("game over!\n"); i=(i+1)%NUM) {
		if (!arr[i]) {
			count++;
			if (count == 3) {
				arr[i] = 1;
				count = 0;
				all_kill++;
				printf("%d\n", all_kill);
			}
		}
	}

	for (i = 0; i < NUM; i++) {
		if (!arr[i]) {
			printf("still live is %d\n",i);
		}
	}

	return 0;
}
