#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

sig_atomic_t okay = 1;

void sig_exit(int arg)
{
    okay = 0;
}

int main(int argc, const char **argv)
{
    int ret;
    // timer Variables
    timer_t mtimr;
    struct itimerspec nval;
    // Signal Variables
    int sig;
    sigset_t set;
    // time Variables
    time_t rawtime;
    struct tm *t = NULL;
    char buff[64] = { 0 };

    // Create time interval of .1 second
    nval.it_interval.tv_sec = 0;
    nval.it_interval.tv_nsec = 100000000;
    nval.it_value.tv_sec = 0;
    nval.it_value.tv_nsec = 1;

    // Block SIGALRM so we can wait for it here
    sigaddset(&set, SIGALRM);
    sigprocmask(SIG_BLOCK, &set, NULL);

    // Handle other signal so we can properly destry the timer
    signal(SIGINT, sig_exit);
    signal(SIGKILL, sig_exit);
    signal(SIGSTOP, sig_exit);
    signal(SIGQUIT, sig_exit);
    signal(SIGTERM, sig_exit);
    // signal(SIGTSTP, sig_exit);

    // Create timer
    timer_create(CLOCK_REALTIME, NULL, &mtimr);
    // Start timer
    timer_settime(mtimr, 0, &nval, NULL);

    while (okay)
    {
        // Block until SIGALRM is recieved
        sigwait(&set, &sig);
        // Get time
        time(&rawtime);
        t = localtime(&rawtime);
        // Format time
        ret = strftime(buff, 64, "\r%d/%m/%Y %I:%M:%S %p %Z", t);
        // Write time unbuffered to the output
        write(STDOUT_FILENO, buff, ret);
    }

    printf("\nExiting\n");

    // Destroy timer
    timer_delete(mtimr);
    exit(0);
}
