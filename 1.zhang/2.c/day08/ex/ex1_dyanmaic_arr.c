#include <stdio.h>
#include <stdlib.h>

#define ROW 4
#define COL 5
int input_str(int **arr, int row, int col)
{
	int i, j;
	for (i = 0; i < row; i++) {
		arr[i] = NULL;
	}

	for (j = 0; j < num; j++) {
		printf("input %d string: \n", j+1);
		i =  0;

		while (1) {
			if (i==4) {
				break;
			}
			p[j] = realloc(p[j],(i+1)*sizeof(int));
			if (NULL == p[j]) {
				break;
			}
			p[j][i] = get_char();
			if (p[i][j]=='\n') {
				p[i][j] = '\0';
				break;
				i++;
			}
		}

	}
}

//动态生成4x5数组
//找出每行最大的地址保存到数组指针
int main(int argc, const char *argv[])
{
	//一个指针指向5个数组
	int (*arr)[5] = NULL;
	int *max_arrs[4] = {NULL, NULL, NULL, NULL};
	input_str(arr, ROW, COL);

	//find_max(max_arr, arr, ROW);

	int i,j;
	for (i = 0; i < ; i++) {

	}


	return 0;
}
