#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <wait.h>

#define MEM_SIZ     1024

int main(void)
{
    char *ptr;
    pid_t pid;
    int shmid;

    shmid = shmget(IPC_PRIVATE, MEM_SIZ, IPC_CREAT | 0600);
    if (shmid < 0) {
        perror("shmget()");
        exit(EXIT_FAILURE);
    }
    pid = fork();
    if (pid < 0) {
        perror("fork()");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        ptr = shmat(shmid, NULL, 0);
        if (ptr == (void *)-1) {
            perror("shmat()");
            exit(EXIT_FAILURE);
        }
        strcpy(ptr, "Hello!");
        shmdt(ptr);
        exit(EXIT_SUCCESS);
    }
    if (pid == 0) {
        wait(NULL);
        ptr = shmat(shmid, NULL, 0);
        puts(ptr);
        shmdt(ptr);
        shmctl(shmid, IPC_RMID, NULL);
    }
    return 0;
}
