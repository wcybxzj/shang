#include <stdio.h>

#define PILSIZE 10

struct piller{
	int stack[PILSIZE];
	int num;
};

//int arr[3] = {1};
struct piller arr[3] = {
	{{6,5,4,3,2,1}, 6},
};

void display(void)
{
	int i, j;

	for (i = 0; i < 3; i++) {
		printf("%c:", 'A'+i);
		for (j = 0; j < arr[i].num; j++) {
			printf("%d ", arr[i].stack[j]);
		}
		for (; j < PILSIZE; j ++) {
			printf("- ");
		}
		printf("\n");
	}
}

int push(int val, int dest)
{
	//is_full
	arr[dest].stack[arr[dest].num] = val;
	arr[dest].num ++;
	return 0;
}

int pop(int src, int *num)
{
	//is_empty()
	arr[src].num --;
	*num = arr[src].stack[arr[src].num];	

	return 0;
}

void move(int src, int dest)
{
	int val;

	pop(src, &val);
	push(val, dest);	

	display();
	getchar();
}

void hannoi(int src, int dest, int temp, int n)
{
	if (n == 1) {
		move(src, dest);
		return;
	}

	hannoi(src, temp, dest, n - 1);	
	move(src, dest);
	hannoi(temp, dest, src, n - 1);
}

int main(void)
{
	display();
	getchar();

	hannoi(0, 1, 2, 6);

	return 0;
}

