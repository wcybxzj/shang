#include <stdio.h>

#define STACKSIZE 32

static int stack[STACKSIZE];
static int top;

int is_full(void)
{
	if (top == STACKSIZE)
		return 1;
	return 0;
}

int is_empty(void)
{
	return top == 0;
}

int push(int val)
{
	if (is_full())
		return -1;
	stack[top++] = val;
	return 0;
}

int pop(void)
{
	if (is_empty()) {
		return -1;
	}
	return stack[--top];
}

void traval(void)
{
	int i;

	for (i = 0; i < top || !printf("\n"); i++) {
		printf("%d ", stack[i]);
	}
}

int step(int n)
{
	int sum = 0;
	int i;

	if (n == 0) {
		traval();
		sleep(1);
		return 1;
	}
	if (n < 0) {
		return 0;
	}
	for (i = 1; i <= 3; i++) {
		push(i);
		sum += step(n - i);
		pop();
	}

	return sum;
}

int main(void)
{
	printf("*****%d*****\n", step(9));

	return 0;
}
