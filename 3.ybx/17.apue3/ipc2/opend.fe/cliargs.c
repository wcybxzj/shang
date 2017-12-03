#include "opend.h"

int cli_args(int argc, char *argv[]){
	if (argc!=3 || strcmp(argv[0], CL_OPEN)!=0) {
		strcpy(errmsg, "usage: open <pathname> <oflag>");
		return -1;
	}

	pathname= argv[1];
	oflag = argv[2];
	return 0;
}
