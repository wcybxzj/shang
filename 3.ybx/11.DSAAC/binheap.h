#ifndef _BINHEAP_H_
#define _BINHEAP_H_

typedef int ElementType;
struct HeapStruct;//在testheap.c中定义

typedef struct HeapStruct *PriorityQueue;
PriorityQueue Init(int max);
void Destroy(PriorityQueue H);
void MakeEmpty(PriorityQueue H);
void Insert(ElementType val, PriorityQueue H);
ElementType DeleteMin(PriorityQueue H);
ElementType FindMin(PriorityQueue H);
int IsEmpty(PriorityQueue H);
int IsFull(PriorityQueue H);

#endif
