#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include "sem.h"

struct sem_st
{
    int val;
    pthread_mutex_t mut;
    pthread_cond_t cond;
};
sem_t * sem_init(int val)
{
    struct sem_st *sem;
    sem = malloc(sizeof(struct sem_st));
    pthread_mutex_init(&sem->mut, NULL);
    pthread_cond_init(&sem->cond, NULL);
    sem->val = val;
    return sem;
}
int sem_destroy(sem_t *sem)
{
    struct sem_st *se = sem;
    if (se == NULL)
        return -EINVAL;
    pthread_mutex_destroy(&se->mut);
    pthread_cond_destroy(&se->cond);
    free(sem);
    return 0;
}
int sem_add(sem_t* sem,  int val)
{
    struct sem_st *se = sem;
    if (se == NULL){
        return -EINVAL;
    }

    pthread_mutex_lock(&se->mut);
    se->val += val;
    pthread_cond_broadcast(&se->cond);
    pthread_mutex_unlock(&se->mut);
    return val;
}
int sem_wait(sem_t* sem, int val)
{
    struct sem_st *se = sem;
    if (se == NULL){
        return -EINVAL;
    }

    pthread_mutex_lock(&se->mut);
    while (se->val < val) {
        pthread_cond_wait(&se->cond, &se->mut);
    }

    se->val -= val;
    pthread_mutex_unlock(&se->mut);
    return val;
}
