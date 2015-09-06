#include <stdio.h>

#define PILSIZE 10

struct piller{
	int stack[PILSIZE];
	int num;
};
int top_a = 6;
int top_b = 0;
int top_c = 0;


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


void move(n, piller_arr[0], piller_arr[1], piller_arr[2] )
{
	if (n==1) {
		return 1;
		//a出
		//c入 

	}


}

int main(void)
{

	display();

	return 0;
}

