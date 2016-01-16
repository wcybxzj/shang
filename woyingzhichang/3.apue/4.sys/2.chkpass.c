#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <shadow.h>
#include <string.h>

int main(int argc,char **argv)
{
	struct spwd *shd_line;
	char *input_pass;
	char *crypted_pass;

	if(argc < 2)
	{
		fprintf(stderr,"Usage...\n");
		exit(1);
	}
	
	input_pass = getpass("PASSWORD:");
	/*if error*/

	shd_line = getspnam(argv[1]);
	/*if error*/

	crypted_pass = crypt(input_pass,shd_line->sp_pwdp);
	/*if error*/	

	if(strcmp(shd_line->sp_pwdp,crypted_pass) == 0)
		puts("ok!");
	else
		puts("Error!");


	exit(0);
}


