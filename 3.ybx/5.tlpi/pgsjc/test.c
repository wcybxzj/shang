
#include <signal.h>
#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{

    for (int i = 0; i < 1000; i++)
    {
        printf("i:%d\n", i);
        sleep(1);
    }
    


}
