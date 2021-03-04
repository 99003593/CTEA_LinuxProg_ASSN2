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
    struct timeval timer_usec;
    struct tm *t = NULL;
    char buff[64] = { 0 };
    char buff_dt[32] = { 0 };
    char buff_tz[16] = { 0 };

    // Create time interval of 1 millisecond
    nval.it_interval.tv_sec = 0;
    nval.it_interval.tv_nsec = 1000000;
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
        gettimeofday(&timer_usec, NULL);
        t = localtime(&timer_usec.tv_sec);
        // Format time
        ret = strftime(buff_dt, 32, "%d/%m/%Y %I:%M:%S", t);
        ret = strftime(buff_tz, 16, "%p %Z", t);
        ret = snprintf(buff, 64, "\r%s.%02ld %s", buff_dt, timer_usec.tv_usec/10000, buff_tz);
        // Write time unbuffered to the output
        write(STDOUT_FILENO, buff, ret);
    }

    printf("\nExiting\n");

    // Destroy timer
    timer_delete(mtimr);
    exit(0);
}
