#include <stdio.h>
#include "binheap.h"
#include <stdlib.h>

#define MIN_PQ_CAPACITY (10) //容量下限
#define MIN_DATA (-32767)

struct HeapStruct {
	int capacity;//容量
	int size;//当前个数
	ElementType * arr;
};

PriorityQueue
Init(int capacity){
	PriorityQueue H;
	if (capacity <  MIN_PQ_CAPACITY) {
		printf("capacity is too small\n");
		exit(1);
	}

	H = malloc(sizeof(struct HeapStruct));
	if (H == NULL) {
		perror("malloc");
		exit(1); 
	} 

	H->arr = malloc((capacity+1) * sizeof(ElementType));
	if (H->arr == NULL) {
		perror("malloc");
		exit(1); 
	}
	H->capacity = capacity;
	H->size = 0;
	H->arr[0] = MIN_DATA;
	return H;
}

void 
MakeEmpty(PriorityQueue H)
{
	H->size = 0;
}

void 
Insert(ElementType val, PriorityQueue H)
{
	int i;
	if (IsFull(H)) {
		printf("PQ is full\n");
		exit(1);
	}
	for (i = ++H->size; val < H->arr[i/2]; i/=2) {
		H->arr[i] = H->arr[i/2];
	}
	H->arr[i] = val;
}

ElementType
DeleteMin(PriorityQueue H)
{
	int i, child;
	ElementType MinElement, LastElement;

	if (IsEmpty(H)) {
		printf("PQ is empty\n");
		exit(1);
	}

	MinElement = H->arr[1];
	LastElement = H->arr[H->size--];

	for (i = 1; 2*i <= H->size; i=child) {
		child = 2*i;
		if (child < H->size && H->arr[child] > H->arr[child+1]) {
			child++;
		}
		if (LastElement > H->arr[child]) {
			H->arr[i] = H->arr[child];
		}else{
			break;
		}
	}
	H->arr[i] = LastElement;
	return MinElement;
}


ElementType
FindMin(PriorityQueue H)
{
	if (IsEmpty(H)) {
		printf("PQ is empty\n");
		exit(1);
	}
	return H->arr[1];
}

int 
IsEmpty(PriorityQueue H)
{
	return H->size == 0;
}

int
IsFull(PriorityQueue H)
{
	return H->size == H->capacity;
}

void 
Destroy(PriorityQueue H)
{
	free(H->arr);
	free(H);
}



