#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid;

    puts("begin");
    fflush(NULL);

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }
    if (pid == 0) {
        execl("./few","test", NULL);
        perror("execl()");
        exit(1);
    }

    wait(NULL);

    puts("end!");
    exit(0);
}
