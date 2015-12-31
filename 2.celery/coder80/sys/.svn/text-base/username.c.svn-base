#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>

int main(int argc,char **argv)
{
	struct passwd *cur;

	if(argc < 2)
	{
		fprintf(stderr,"Usage...\n");
		exit(1);
	}

	cur = getpwuid(atoi(argv[1]));
	/*if error*/

	puts(cur->pw_name);

	exit(0);
}


