#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sem.h"

#define LEFT    30000000
#define RIGHT   30000200
#define THRNUM  4

static sem_t *sem;

static void *thr_prime(void *p)
{
    int i;
    int make = 1;
    i = (int)p;

    for (int j = 2; j < i / 2; j++) {
        if (i % j == 0) {
            make = 0;
            break;
        }
    }
    if (make)
        printf("%d is primer.\n", i);

    sem_add(sem, 1);
    pthread_exit(NULL);
    return NULL;
}


int main(void)
{
    pthread_t tid[RIGHT - LEFT + 1];
    int err;
    sem = sem_init(THRNUM);

    for (int i = LEFT; i <= RIGHT; i++) {
        sem_wait(sem, 1);
        err = pthread_create(tid+i-LEFT, NULL, thr_prime, (void *)i);
        if (err) {
            fprintf(stderr, "pthread_create fail: %s\n", strerror(err));
            exit(EXIT_FAILURE);
        }
    }

    for (int i = LEFT; i <= RIGHT; i++) {
        pthread_join(tid[i-LEFT], NULL);
    }
    sem_destroy(sem);
    return 0;
}
