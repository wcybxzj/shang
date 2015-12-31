#include <stdio.h>
#define N 8
void bubble_sort(int arr[], int n)
{
	int i, j, k, l;
	for (i = n; i > 0 ; i--) {
		//printf("i %d\n", i);//7->1
		j = 0; k = i;
		for (; j < k ;) {
			 l = j+1;
			 if (l==n) {
				 break;
			 }
			 //printf("%d %d,",j ,l);
			 if (arr[j] > arr[l]) {
			     arr[j] = arr[j] ^ arr[l];
			     arr[l] = arr[j] ^ arr[l];
			     arr[j] = arr[j] ^ arr[l];
			 }
			 j++;
		}
		//printf("\n");
	}
	return;
}

int main(int argc, const char *argv[])
{
	int i;
	int d[N]={48,38,65,97,76,13,27,49};
	bubble_sort(d, N);
	for (i = 0; i < N; i++) {
		printf("%d \n",d[i]);
	}
	return 0;
}
