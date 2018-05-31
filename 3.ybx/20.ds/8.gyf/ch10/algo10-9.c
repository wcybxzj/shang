#include <stdio.h>
typedef int InfoType;
#include "c9.h"
#include "c10-1.h"
typedef SqList HeapType;

#define N 8

void print(HeapType H)
{
	int i;
	for (i = 1; i <= H.length; i++) {
		printf("(%d, %d)", H.r[i].key, H.r[i].otherinfo);
	}
	printf("\n");
}

void HeapAdjust(HeapType *H, int s, int h)
{
	RedType tmp;
	int target_index;
	
	int index = 2*s;
	while (index <= h) {
		target_index = index/2;
		if (H->r[index].key > H->r[target_index].key) {
				target_index = index;
		}
		if (index+1 <= h) {
			if (H->r[index+1].key > H->r[target_index].key) {
				target_index = index+1;
			}
		}
		if (target_index==index/2) {
			break;
		}
		if (H->r[target_index].key > H->r[index/2].key) {
			tmp = H->r[index/2];
			H->r[index/2] = H->r[target_index];
			H->r[target_index] = tmp;
		}
		index*=2;
	}
}

void HeapSort(HeapType *H)
{
	RedType tmp;
	int i;
	//建堆
	for (i = (H->length)/2; i >0; i--) {
		HeapAdjust(H, i, H->length);
	}
	for (i = H->length; i >1; i--) {
		tmp = H->r[1];
		H->r[1] = H->r[i];
		H->r[i] = tmp;
		HeapAdjust(H, 1, i-1);
	}
}

int main(int argc, const char *argv[])
{
	int i;
	RedType d[N] = { {49, 1}, {38, 2}, {65, 3}, {97, 4}, {76, 5}, {13, 6}, {27, 7}, {49, 8} };
	HeapType heap;
	heap.length = N;
	//h.r的0号元素,将空着
	for (i = 0; i < N; i++) {
		heap.r[i+1]= d[i];
	}

	printf("before\n");
	print(heap);
	HeapSort(&heap);

	printf("after\n");
	print(heap);

	return 0;
}
