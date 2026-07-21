#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
    pid_t pid;

    printf("[%d] Begin\n", getpid());
    pid = fork();
    if (pid < 0) {
        perror("fork()");
        exit(1);
    }
    if (pid == 0)
    {
        printf("[%d]: chile is working!parent:[%d]\n", getpid(), getppid());
    }
    else
    {
        printf("[%d]: parent is working!parent:[%d]\n", getpid(), getppid());
    }
    printf("[%d] end!\n", getpid());
    getchar();
    exit(0);
}