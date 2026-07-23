#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

static void *func(void *arg)
{
    printf("thread start succ!\n");
    // return NULL;
    pthread_exit(NULL);
}
int main(void)
{
    pthread_t tid;
    int err;

    puts("begin!");
    err = pthread_create(&tid, NULL, func, NULL);
    if (err) {
        fprintf(stderr, "%s\n", strerror(err));
        exit(EXIT_FAILURE);
    }

    pthread_join(tid, NULL);
    puts("End!");
    return 0;
}
