#include <stdio.h>

//1-----白方   2-------黑方
int arr[10][10] = {};

int color = 1;

void print_grp(void)
{
	int i, j;
		
	printf("   1 2 3 4 5 6 7 8 9 10\n");
	for (i = 0; i < 10; i++) {
		printf("%-3d", i+1);
		for (j = 0; j < 10; j++) {
			if(arr[i][j] == 0) {
				printf("+ ");
			} else if (arr[i][j] == 1) {
				printf("w ");
			} else {
				printf("b ");
			}
		}
		printf("\n");
	}
}

void input_qz(void)
{
	int x, y;

	if (color == 1) {
		printf("请白方输入：");
	} else {
		printf("请黑方输入：");
	}
	scanf("%d%d", &x, &y);

	if (x>=1&&x<=10&&y>=1&&y<=10 && arr[x-1][y-1]==0) {
		arr[x-1][y-1] = color;		
		if (color == 1) {
			color = 2;
		} else {
			color = 1;
		}
	}
}

int main(void)
{
	system("clear");
	print_grp();
	while (1) {
		input_qz();
		system("clear");
		print_grp();
#if 0
		if (is_win()) {
			break;
		}
#endif
	}

	return 0;
}
