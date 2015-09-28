#include <stdio.h>
#include <stdlib.h>

#define COL_CANVAS 4

typedef struct{
	char (*arr)[COL_CANVAS];
	int current_pos;
}block,*block_entry_p;


int main(int argc, const char *argv[])
{
	block_entry_p block_entry = malloc(sizeof(block));
	block_entry->current_pos = 1;
	block_entry->arr = calloc(1,sizeof(char) * COL_CANVAS *16);//4x16=56

	block_entry->arr[0][1] = 1;
	block_entry->arr[1][0] = 1;
	block_entry->arr[1][1] = 1;
	block_entry->arr[1][2] = 1;

	int i,j;

	for (i = 0; i <4; i++) {
		for (j = 0; j < COL_CANVAS; j++) {
			if ( block_entry->arr[i][j] == 1 ) {
				printf("i %d j %d",i ,j);
				printf(" =%d |",block_entry->arr[i][j]);
			}
		}
	}

	return 0;
}
