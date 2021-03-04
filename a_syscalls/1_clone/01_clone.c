#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <utime.h>
#include <stdio.h>
#include <stdlib.h>

#define CP_BLK_SIZ (512)

static const char msg_help[] = "Usage:\n\tclone srcfile dstfile\n";

unsigned char buff[CP_BLK_SIZ];

int main(int argc, const char **argv)
{
    int ret = -1, fd_src = -1, fd_dst = -1;
    ssize_t status = 1, len_wr = 0;

    struct stat stat_buff;
    struct timespec times[2] = { 0 };

    if (argc < 3)
    {
        fprintf(stderr, msg_help);
        ret = -1;
        goto exit_end;
    }

    fd_src = open(argv[1], O_RDONLY);
    if (fd_src < 0)
    {
        perror("open");
        ret = -1;
        goto exit_end;
    }

    fd_dst = open(argv[2], O_WRONLY | O_CREAT, 0644);
    if (fd_src < 0)
    {
        perror("open");
        ret = -1;
        goto exit_close_src;
    }

    while(status)
    {
        status = read(fd_src, buff, CP_BLK_SIZ);
        if (status < 0)
        {
            perror("write");
            ret = -1;
            goto exit_close_dst;
        }
        else if (status == 0)
        {
            break;
        }
        else
        {
            status = write(fd_dst, buff, status);
            if (status < 0)
            {
                perror("write");
                ret = -1;
                goto exit_close_dst;
            }
            else
            {
                len_wr += status;
            }
        }
    }

    printf("Wrote %ld bytes\n", len_wr);

    fstat(fd_src, &stat_buff);
    if (fchmod(fd_dst, stat_buff.st_mode) != 0)
    {
        perror("chmod");
        ret = -1;
        goto exit_close_dst;
    }

    times[0] = stat_buff.st_atim;
    times[1] = stat_buff.st_mtim;

    if (futimens(fd_dst, (const struct timespec *)&times) != 0)
    {
        perror("futimens");
        ret = -1;
        goto exit_close_dst;
    }

    ret = 0;

exit_close_dst:
    close(fd_dst);
exit_close_src:
    close(fd_src);
exit_end:
    exit(ret);
}