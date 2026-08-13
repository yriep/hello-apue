#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid;
    putchar('c');
    puts("begin");
    fflush(NULL);

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }
    if (pid == 0) {
        execl("/bin/date","date", "+%s", NULL);
        perror("execl()");
        exit(1);
    }

    wait(NULL);

    puts("end!");
    exit(0);
}
