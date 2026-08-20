#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <wait.h>


#define FNAME   "/tmp/out"
#define BUFSIZE 32
union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};

static void P(int semid)
{
    struct sembuf sop;

    sop.sem_num = 0;
    sop.sem_op = -1;
    sop.sem_flg = 0;
    semop(semid, &sop, 1);
}
static void V(int id)
{
    struct sembuf sop;

    sop.sem_num = 0;
    sop.sem_op = 1;
    sop.sem_flg = 0;
    semop(id, &sop, 1);

}
static void add_func(int semid)
{
    FILE *fp;
    char buf[BUFSIZE];

    fp = fopen(FNAME, "r+");
    if (fp == NULL) {
        perror("fopen()");
        exit(EXIT_FAILURE);
    }
    P(semid);
    fgets(buf, BUFSIZE, fp);
    fseek(fp, 0, SEEK_SET);
    fprintf(fp, "%d\n", atoi(buf) + 1);
    fflush(fp);
    V(semid);

    fclose(fp);
}
int main(void)
{
    int semid;
    pid_t pid;
    union semun arg;

    semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    if (semid < 0) {
        perror("semget()");
        exit(EXIT_FAILURE);
    }

    arg.val = 1;
    semctl(semid, 0, SETVAL, arg.val);

    for (int i = 0; i < 20; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork()");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            add_func(semid);
            exit(EXIT_SUCCESS);
        }
    }
    for (int i = 0; i < 20; i++) {
        wait(NULL);
    }

    semctl(semid, 0, IPC_RMID);
    return 0;
}
