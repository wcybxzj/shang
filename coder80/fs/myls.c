#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <glob.h>
#include <string.h>

#define A_POS 0
#define L_POS 1
#define I_POS 2
#define N_POS 3

 //用宏得到某数的某位
#define is_bitmap(Number,pos) ((Number) >> (pos)&1)

//bitmap = bitmap | 1u<<3;
void set_bitmap(int *bitmap, int num){
	*bitmap = *bitmap | 1u << num;
}

int main(int argc, char **argv)
{
	int i, ch, bitmap;
	char *str = "*";

	bitmap = 0;
	while (1) {
		ch = getopt(argc, argv, "-lian");
		if (ch<0) {
			break;
		}
		switch(ch){
			case 1:
				break;
			case 'a':
				set_bitmap(&bitmap,0);
				break;
			case 'l':
				set_bitmap(&bitmap,1);
				break;
			case 'i':
				set_bitmap(&bitmap,2);
				break;
			case 'n':
				set_bitmap(&bitmap,3);
				break;
			default:
				break;
		}
	}

	printf("%d\n", bitmap);

	if () {
		/* code */
	}


	//glob_t globres;
	//glob(str,0,NULL,&globres);
	//for (i = 0; i < globres.gl_pathc; i++) {
	//	printf("%s ",globres.gl_pathv[i]);
	//}

	////判断i
	//if () {
	//	/* code */
	//}


	//printf("\n");

	return 0;
}
