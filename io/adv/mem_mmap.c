#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

/*
 * 进程间通信
 *
 * */
int main(void)
{
    pid_t pid;
    char *ptr;

    ptr = mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap()");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork()");
        munmap(ptr, 1024);
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        strncpy(ptr, "hello world", 5);
        munmap(ptr, 1024);
        exit(EXIT_SUCCESS);
    } else {
        wait(NULL);
        puts(ptr);
    }
    munmap(ptr, 1024);
    return 0;
}
