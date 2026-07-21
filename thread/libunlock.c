#include <pthread.h>
#include <stdio.h>
#include <time.h>

static void *start_routine(void *p)
{
    for (int i = 0; i < 5; i++) {
        putchar((int)p);
    }
    pthread_exit(NULL);
}
int main(void)
{
    pthread_t tid[5];
    for (int i = 0; i < 5; i++) {
        pthread_create(tid + i, NULL, start_routine, (void *)('a' + i));
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(tid[i], NULL);
    }
    return 0;
}
