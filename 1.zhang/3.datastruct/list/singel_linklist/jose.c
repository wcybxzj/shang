#include <stdio.h>
#include <list.h>

static void print_fun(const void *data){
	const int *d = data;
	printf("打印%d\n",*d);

}

int main(int argc, const char *argv[])
{
	int i;
	LIST *jose;
	int kill = 0;
	int cnt =0;


	jose = init_head(sizeof(int));
	
	for (i = 0; i < 43; i++) {
		insert_list(jose, &i, REAR);
	}

	traval_list(jose, print_fun);

	printf("killing\n");
	for (i = 1; kill < 41 || !printf("game over\n"); i = (i+1)%43) {
		cnt++;
		if (cnt == 3) {
			arr[i] = 1;
			printf("%d is killed\n", i);
			cnt = 0;
			kill ++;
		}
	}


	return 0;
}
