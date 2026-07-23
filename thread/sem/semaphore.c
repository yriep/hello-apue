#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

static int n;
static sem_t sem1;
static sem_t sem2;

static void *getnum(void *p)
{
    for (int i = 0; i < 5; i++) {
        printf("input num: ");
        sem_wait(&sem2);
        scanf("%d", &n);
        sem_post(&sem1);
    }
    return 0;
}

static void *accu(void *p)
{
    int tol = 0;
    for (int i = 0; i < 5; i++) {
        sem_wait(&sem1);
        tol += n;
        sem_post(&sem2);
    }
    printf("total num is %d\n", tol);
    return (void *)tol;
}
int main(int argc, char *argv[])
{
    void *a;
    void *b;
    pthread_t tid1, tid2;

    sem_init(&sem1, 0, 0);
    sem_init(&sem2, 0, 1);
    pthread_create(&tid1, NULL, getnum, NULL);
    pthread_create(&tid2, NULL, accu, NULL);

    pthread_join(tid1, &a);
    pthread_join(tid2, &b);

    printf("thread 1 return %d, thread 2 return : %d\n", (int)a, (int)b);
    sem_destroy(&sem1);
    sem_destroy(&sem2);
    return 0;
}
