#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include <signal.h>

static const char msg_help[] = "Usage:\n\tsigsend signum pid\n";

int main(int argc, const char **argv)
{
    int ret = -1;
    char *endp;
    long num;
    pid_t pid;
    int signum;

    if (argc < 2)
    {
        fprintf(stderr, msg_help);
        ret = -1;
        goto exit_end;
    }

    errno = 0;
    signum = strtol(argv[1], &endp, 10);

    if (errno != 0 || signum < 1 || signum > 64)
    {
        fprintf(stderr, "Invalid signal id: %d (%s) | %d\n", signum, argv[1], errno);
        ret = -1;
        goto exit_end;
    }

    num = strtol(argv[2], &endp, 10);
    pid = num;

    if (errno != 0 || pid != num)
    {
        fprintf(stderr, "Invalid process id: %s\n", argv[2]);
        ret = -1;
        goto exit_end;
    }

    if (kill(pid, signum) != 0)
    {
        perror("kill");
        ret = -1;
    }

    ret = 0;
exit_end:
    exit(ret);
}