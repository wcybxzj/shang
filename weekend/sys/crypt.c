#include <stdio.h>
#include <stdlib.h>
#include <shadow.h>
#include <unistd.h>

int main(int argc,char **argv)
{
	struct spwd *shdowline;
	char *inputpass,*cryptedpass;


	if(argc < 2)
	{
		fprintf(stderr,"Usage....\n");
		exit(1);
	}

	inputpass = getpass("PASSWD:");
	/*if error*/

	shdowline = getspnam(argv[1]);
	if(shdowline == NULL)
	{
		perror("getspnam()");
		exit(1);
	}

	cryptedpass = crypt(inputpass,shdowline->sp_pwdp);
	/*if error*/
	
	if(strcmp(cryptedpass,shdowline->sp_pwdp) == 0)
		puts("Ok!");	
	else
		puts("Failed.");





	exit(0);
}



