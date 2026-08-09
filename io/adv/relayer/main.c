#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "relayer.h"

int main(void)
{
    int fd1, fd2, fd3, fd4;
    int rel1, rel2;

    fd1 = open("/dev/tty10", O_RDWR);
    fd2 = open("/dev/tty11", O_RDWR);
    fd3 = open("/dev/tty12", O_RDWR);

    write(fd1, "tty1", 5);
    write(fd2, "tty2", 5);
    write(fd3, "tty3", 5);
    rel1 = relay_addjob(fd1, fd2);
    if (rel1 < 0) {
        fprintf(stderr, "%s\n", strerror(errno));
    }

    rel2 = relay_addjob(fd2, fd3);
    if (rel2 < 0) {
        fprintf(stderr, "%s\n", strerror(errno));
    }

    while (1) {
        pause();
    }
    close(fd1);
    close(fd2);
    close(fd3);
    return 0;
}
