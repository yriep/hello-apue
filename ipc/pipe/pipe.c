#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define     SIZE    32

int main(void)
{
    pid_t pid;
    int   pfd[2];
    char buf[SIZE];

    if (pipe(pfd) < 0) {
        perror("pipe()");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork()");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {         // parent for read;
        close(pfd[1]);
        read(pfd[0], buf, SIZE - 1);
        write(1, buf, strlen(buf));
        wait(NULL);
    } else {
        close(pfd[0]);
        write(pfd[1], "hello!", 6);
        exit(0);
    }

    return 0;
}
