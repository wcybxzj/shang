#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{
	union msg_st{
		int a;
		struct {
			int bb;
			int cc;
		}st;
	}abc;

	abc.a = 12;
	printf("bb is %d\n",abc.st.bb);

	return 0;
}
