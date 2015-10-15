#include <stdio.h>
#include <stdlib.h>



//glob
//stat


int main(int argc, const char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage...\n");
		exit(1);
	}

	mydu(argv[1]);

	return 0;
}
