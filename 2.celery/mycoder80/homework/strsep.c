#include <stdio.h>
#include <string.h>

int main(int argc, const char *argv[])
{
	char *tok;
	char *line;
	size_t line_num;
	getline(&line, &line_num, stdin);
	while (1) {
		tok = strsep(&line, " \n");
		if (tok==NULL) {
			break;
		}

		if (tok[0]=='\0') {
			continue;
		}

		printf("tok[0]:%s\n", tok);
	}
	return 0;
}
