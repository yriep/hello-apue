#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "mytbf.h"

#define BURST       100
#define CPS         10
#define BUF_SIZE    1024

int main(int argc, char *argv[])
{
    int fd;
    int str_len = 0;
    int ret = 0;
    char buf[BUF_SIZE];
    mytbf_t *tbf_t = NULL;
    int token = 0;


    if (argc < 2) {
        fprintf(stderr, "Usage <%s> <F>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    while ((fd = open(argv[1], O_RDONLY)) < 0) {
        if (errno != EINTR) {
            perror("open()");
            exit(EXIT_FAILURE);
        }

    }

    tbf_t = mytbf_init(CPS, BURST);
    if (tbf_t == NULL) {
        fprintf(stderr, "mytbf_init() error");
        exit(EXIT_FAILURE);
    }

    do {
        token = tbftoken_fetch(tbf_t, BUF_SIZE);
        if (token < 0) {
            fprintf(stderr, "tbftoken_fetch err(%s)", strerror(-token));
            exit(EXIT_FAILURE);
        }
        while((str_len = read(fd, buf, token)) < 0) {
            if (errno == EINTR)
                continue;
            perror("read()");
            exit(EXIT_FAILURE);
        }

        if (str_len == 0)
            break;

        if (token - str_len > 0)
            tbftoken_retur(tbf_t, token - str_len);

        int str_pos = 0;
        while(str_len > 0) {
            ret = write(1, buf + str_pos, str_len);
            if (ret < 0) {
                if (errno == EINTR)
                    continue;

                perror("write()");
                exit(EXIT_FAILURE);
            }
            str_pos += ret;
            str_len -= ret;
        }
    } while (1);

    close(fd);
    tbf_destroy(tbf_t);
    return 0;
}
