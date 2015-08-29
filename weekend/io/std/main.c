#include <stdio.h>
#include <stdlib.h>


int main(int argc,char *argv[] /*char **argv*/)
{
	int a,ret;
	char str[12]; 
	

	scanf("%s",str);


	while(1)
	{
		
		ret = scanf("%d",&a);
		if(ret != 1)
			break;

		printf("a = %d\n",a);
	}



#if 0
	char *str1 = "now";
	char str[128] = "a12abc3";
	int year = 2015,mon = 6,day = 13;

	printf("%d\n",atoi(str));

	sprintf(str,"%s:%d-%d-%d\n",str1,year,mon,day);

	puts(str);
#endif




#if 0
	int i;
	printf("argc = %d\n",argc);	

	for(i = 0 ; i < argc; i++)
		puts(argv[i]);
#endif

	
//	printf("Hello!\n");






	exit(0);
}



