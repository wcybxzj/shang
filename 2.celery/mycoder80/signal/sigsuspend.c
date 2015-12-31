#include <stdio.h>
static int count = 0;
void sig_handler(int s, siginto_t * sig_p, void *unsed){
	write(1, "!", 1);
	count++;
}

int main(int argc, const char *argv[])
{
	int i, j;
	sigset_t set, oset, saveset;
	struct sigaction sa, o_sa;

	for (j = 0; j < 10000; j++) {
		if (j==0) {
			for (i = 0; i < 5; i++) {
				write(1, "*", 1);
				sleep(1);
			}
		}else if(j==1){
			if (count==0) {
				sa.sigaction = sig_handler;
				sigempty(&sa.sa_mask);
				sa.sa_mask = 0;
				sigaction(SIGALRM, &sa, &osa);
			}elseif(){
			
			}


			for (i = 0; i < 5; i++) {
				write(1, "*", 1);
				sleep(1);
			}
		}else if(j==2){
		
		
		}

		printf("\n");
	}
	return 0;
}
