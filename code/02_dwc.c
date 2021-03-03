#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define CP_BLK_SIZ (512)

static const char msg_help[] = "Usage:\n\twc file\n";

unsigned char buff[CP_BLK_SIZ];

static size_t cc, wc, lc;

void process_buff(unsigned char *buff, int len)
{
    static int prev_bak = 0;
    register int prev_space = prev_bak;
    for(int i=0; i < len; i++)
    {
        if (isspace(buff[i]) && prev_space == 0)
        {
            wc++;
            prev_space = 1;
        }
        else
        {
            prev_space = 0;
        }
        if (buff[i] == '\n')
            lc++;
    }
    cc += len;
    prev_bak = prev_space;
}

int main(int argc, const char **argv)
{
    int ret = -1, fd_file = -1;
    ssize_t status = 1;

    if (argc < 2)
    {
        fprintf(stderr, msg_help);
        ret = -1;
        goto exit_end;
    }

    fd_file = open(argv[1], O_RDONLY);
    if (fd_file < 0)
    {
        perror("open");
        ret = -1;
        goto exit_end;
    }

    cc = 0;
    wc = 0;
    lc = 0;

    while(status)
    {
        status = read(fd_file, buff, CP_BLK_SIZ);
        if (status < 0)
        {
            perror("write");
            ret = -1;
            goto exit_close;
        }
        else if (status == 0)
        {
            break;
        }
        else
        {
            process_buff(buff, status);
        }
    }

    printf("%lu %lu %lu %s\n", lc, wc, cc, argv[1]);

exit_close:
    close(fd_file);
exit_end:
    exit(ret);
}