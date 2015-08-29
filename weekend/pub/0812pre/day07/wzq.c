#include <stdio.h>

//1-----白方   2-------黑方
int arr[10][10] = {};

int color = 1;
int x, y;

void print_grp(void)
{
	int i, j;
		
	printf("   1 2 3 4 5 6 7 8 9 10\n");
	for (i = 0; i < 10; i++) {
		printf("\33[45m%-3d", i+1);
		printf("\33[0m");
		for (j = 0; j < 10; j++) {
			if(arr[i][j] == 0) {
				printf("\33[32m+ ");
			} else if (arr[i][j] == 1) {
				printf("\33[34mw ");
			} else {
				printf("\33[30mb ");
			}
			printf("\33[0m");
		}
		printf("\n");
	}
}

void input_qz(void)
{
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

int is_win(void)
{
	//x y
	int i, j;
	int sum;

	sum = 1;
	for (j = y-1, i = x-2; i >= 0; i--) {
		if (arr[i][j] != arr[x-1][y-1]) {
			break;
		}	
		sum ++;
	}
	for (j = y-1, i = x; i < 10; i++) {
		if (arr[i][j] != arr[x-1][y-1]) {
			break;
		}	
		sum ++;
	}
	if (sum >= 5) {
		return 1;
	}

	return 0;		
}

int main(void)
{
	system("clear");
	print_grp();
	while (1) {
		input_qz();
		system("clear");
		print_grp();

		if (is_win()) {
			break;
		}
	}
	system("clear");
	if (color == 1) {
		printf("\33[5;10H黑方获胜\n");
	}else {
		printf("\33[5;10H白方获胜\n");
	}

	return 0;
}
