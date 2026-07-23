#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define LEFT    30000000
#define RIGHT   30000200
#define THRNUM  (RIGHT - LEFT + 1)

struct st_store
{
    int n;
};

static void *thr_prime(void *p)
{
    int i;
    int make = 1;
    i = ((struct st_store *)p)->n;

    for (int j = 2; j < i / 2; j++) {
        if (i % j == 0) {
            make = 0;
            break;
        }
    }
    if (make)
        printf("%d is primer.\n", i);
    pthread_exit(p);
    return NULL;
}

int main(void)
{
    pthread_t tid[THRNUM];
    int err;
    struct st_store *s;
    void *ptr;

    for (int i = LEFT; i <= RIGHT; i++) {
        s = malloc(sizeof(struct st_store));
        s->n = i;
        err = pthread_create(tid+i-LEFT, NULL, thr_prime, s);
        if (err) {
            fprintf(stderr, "pthread_create fail: %s\n", strerror(err));
            exit(EXIT_FAILURE);
        }
    }

    for (int i = LEFT; i <= RIGHT; i++) {
        pthread_join(tid[i-LEFT], &ptr);
        // 最好在一个模块或函数 malloc和free
        free(ptr);
    }
    return 0;
}
