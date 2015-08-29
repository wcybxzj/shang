#include <stdio.h>
#if 0  			
				n == 4
			 i			j
	  *      0			j==3           j==i+n-1       
	 * *	 1   	 	j==2   j==4    j==n-i-1   j==i+n-1     
	*   *	 2       	j==1   j==5    j==n-i-1   j==i+n-1
   *     *	 3          j==0   j==6    j==i-(n-1) j==i+n-1
    *   *	 4          j==1   j==5	   j==i-(n-1) j==3(n-1)-i      
	 * *	 5          j==2   j==4    j==i-(n-1) j==3(n-1)-i
	  *      6          j==3           j==i-(n-1)
#endif

int main(void)
{
	int n;
	int i, j;

	printf("input:");
	scanf("%d", &n);

	for (i = 0; i < 2*n-1; i++) {
		for (j = 0; j < 2*n-1; j++) {
			if (j==i-(n-1)||j==i+(n-1)||j==3*(n-1)-i||j==n-i-1) {
				printf("*");
			}else {
				printf(" ");		
			}
		}
		printf("\n");
	}

	return 0;
}
