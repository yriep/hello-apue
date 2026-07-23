#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TRNUM   4
static int num = 0;
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static int next(int n)
{
    n = n + 1;
    if (n % TRNUM == 0)
        n = 0;
    return n;
}

static void *tr_func(void *p)
{
    int i = (int)p;
    int ch = 'a' + i;
    while (1) {
        pthread_mutex_lock(&mut);
        while (num != i) {
            pthread_cond_wait(&cond, &mut);
        }
        write(1, &ch, 1);
        num = next(num);
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mut);
    }
    pthread_exit(NULL);
}

int main(void)
{
    int i, err;
    pthread_t tid[TRNUM];

    for (i = 0; i < TRNUM; i++) {
        err = pthread_create(tid + i, NULL, tr_func, (void *)i);
        if (err) {
            fprintf(stderr, "pthread_create(): %s\n", strerror(err));
        }
    }

    alarm(1);
    for (i = 0; i < TRNUM; i++) {
        pthread_join(tid[i], NULL);
    }
    return 0;
}
