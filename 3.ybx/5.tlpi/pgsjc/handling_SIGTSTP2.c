/* Listing 34-6 */

/* handling_SIGTSTP.c

   Demonstrate the correct way to catch SIGTSTP and raise it again (so that a
   parent process that is monitoring this program can see that it was stopped
   by SIGTSTP).
*/

/*
练习34-5
root@ybx-virtual-machine:~/www/shang/3.ybx/5.tlpi/pgsjc# ./handling_SIGTSTP2
^ZCaught SIGTSTP

[1]+  Stopped                 ./handling_SIGTSTP2
root@ybx-virtual-machine:~/www/shang/3.ybx/5.tlpi/pgsjc# fg
./handling_SIGTSTP2

[1]+  Stopped                 ./handling_SIGTSTP2
root@ybx-virtual-machine:~/www/shang/3.ybx/5.tlpi/pgsjc# fg(需要2次 SIGCONT)
./handling_SIGTSTP2
Exiting SIGTSTP handler
Main
^C
root@ybx-virtual-machine:~/www/shang/3.ybx/5.tlpi/pgsjc# 
*/

#include <signal.h>
#include "tlpi_hdr.h"

static void                             /* Handler for SIGTSTP */
tstpHandler(int sig)
{
    sigset_t tstpMask, prevMask;
    int savedErrno;
    struct sigaction sa;

    savedErrno = errno;                 /* In case we change 'errno' here */

    ////////////////////////////////////////////////////////////////////////////
    /* Unblock SIGTSTP; the pending SIGTSTP immediately suspends the program */
    sigemptyset(&tstpMask);
    sigaddset(&tstpMask, SIGTSTP);
    if (sigprocmask(SIG_UNBLOCK, &tstpMask, &prevMask) == -1)
        errExit("sigprocmask");
    ////////////////////////////////////////////////////////////////////////////

    printf("Caught SIGTSTP\n");         /* UNSAFE (see Section 21.1.2) */

    if (signal(SIGTSTP, SIG_DFL) == SIG_ERR)
        errExit("signal");              /* Set handling to default */

    ////////////////////////////////////////////////////////////////////////////
    raise(SIGTSTP);                     /* Generate a further SIGTSTP */
    ////////////////////////////////////////////////////////////////////////////
    raise(SIGTSTP);                     /* Generate a further SIGTSTP */




    /* Execution resumes here after SIGCONT */
    if (sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1)
        errExit("sigprocmask");         /* Reblock SIGTSTP */

    sigemptyset(&sa.sa_mask);           /* Reestablish handler */
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = tstpHandler;
    if (sigaction(SIGTSTP, &sa, NULL) == -1)
        errExit("sigaction");

    printf("Exiting SIGTSTP handler\n");
    errno = savedErrno;
}

int
main(int argc, char *argv[])
{
    struct sigaction sa;

    /* Only establish handler for SIGTSTP if it is not being ignored */

    if (sigaction(SIGTSTP, NULL, &sa) == -1)
        errExit("sigaction");

    if (sa.sa_handler != SIG_IGN) {
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        sa.sa_handler = tstpHandler;
        if (sigaction(SIGTSTP, &sa, NULL) == -1)
            errExit("sigaction");
    }

    for (;;) {                          /* Wait for signals */
        pause();
        printf("Main\n");
    }
}
