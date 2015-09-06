#include <stdio.h>
#define NUM 10

void bubble(int arr[])
{
	int i;
	//9->1
	for (i = NUM-1; i > 0 ; i--) {
		printf("%d\n", i);
	}
}

int getMin(int arr[] ,int from){
	int i = from;
	int min = arr[i];
	int min_index = i; 
	for (j = i+1; j<NUM ; j++) {
		printf("getMinx i:%d\n", i);
		if (min > arr[j]) {
			min = arr[j];
			min_index = i;
		}
	}
	return i;
}

void select (int arr[])
{
	int i, j, min_index;
	//1->9
	for (i = 1; i < NUM ; i++) {
		j = 0;
		min_index = getMin(arr, i);
		if (arr[j] > arr[min_index] ) {
			arr[j] = arr[j]^arr[min_index];
			arr[min_index] = arr[j]^arr[min_index];
			arr[j] = arr[j]^arr[min_index];
		}
		j++;
	}
}

void pr(int arr[])
{
	int i;
	for (i = 0; i < NUM; i++) {
		printf(" %d ", arr[i]);
	}
	printf("\n");
}


int main(int argc, const char *argv[])
{
	int i;
	int arr[NUM] = {};
	srand(time(NULL));
	for (i = 0; i < NUM; i++) {
		arr[i] = rand()%101;
	}
	pr(arr);
	select(arr);
	//bubble(arr);
	pr(arr);
	return 0;
}
