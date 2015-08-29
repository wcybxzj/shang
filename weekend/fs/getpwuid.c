#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>

int main(int argc , char **argv)
{
	struct passwd *pwline;
	
	if(argc < 2)
	{
		fprintf(stderr,"Usage...\n");
		exit(1);
	}

	pwline = getpwuid(atoi(argv[1]));
	/*if error*/

	puts(pwline->pw_name);
	


	exit(0);
}


