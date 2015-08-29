#include <stdio.h>

int main(void)
{
	int seed;
	int arr[10];
	int i;
	int max, min;
	int m_x, m_i;

	seed = time(NULL);
	srand(seed);

	for (i = 0; i < 10; i++) {
		arr[i] = rand()%100;
		printf("%d ", arr[i]);
	}
	printf("\n");
	max = min = arr[0];
	m_x = m_i = 0;
	for (i = 0; i < 10; i++) {
		if (arr[i] > max) {
			max = arr[i];
			m_x = i;
		}
		if (arr[i] < min) {
			min = arr[i];
			m_i = i;
		}
	}
	arr[m_x] = arr[0];
	arr[0] = max;
	arr[m_i] = arr[9];
	arr[9] = min; 

	for (i = 0; i < 10; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");
	
	printf("max = %d, min = %d\n", max, min);

	return 0;
}
