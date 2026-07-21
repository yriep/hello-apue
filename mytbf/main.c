#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "mytbf.h"

#define CPS     10
#define BURST   100
#define MAXBUF  1024

int main(int argc, char *argv[])
{
    int tbf, len, token;
    int ret = 0;
    int fd;
    char buf[MAXBUF];

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open()");
        exit(EXIT_FAILURE);
    }

    tbf = mytbf_init(CPS, BURST);
    if (tbf < 0) {
        fprintf(stderr, "mytbf_init err: %s\n", strerror(-tbf));
        exit(EXIT_FAILURE);
    }
    while (1) {
        token = tbf_fetchtoken(tbf, BUFSIZ);
        if (token < 0) {
            fprintf(stderr, "fetch token err: %s\n", strerror(-token));
            exit(EXIT_FAILURE);
        }

        while ((len = read(fd, buf, token)) < 0) {
            if (errno == EINTR)
                continue;
            perror("read()");
            exit(1);
        };

        if (token - len > 0)
            tbf_returtoken(tbf, token - len);
        if (len == 0)
            break;

        int strpos = 0;
        while (len > 0) {
            ret = write(1, buf + strpos, len);
            if (ret < 0) {
                if (errno == EINTR)
                    continue;

                perror("write()");
                exit(1);
            }
            len -= ret;
            strpos += ret;
        }

    }

    tbf_destroy(tbf);
    close(fd);
    return 0;
}
