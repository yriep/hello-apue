#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define TRDNUM      4

static pthread_mutex_t mut[TRDNUM];

static int next(int i)
{
    /*
    if (++i == TRDNUM)
        i = 0;
    return i;
    */
    int n = i + 1;
    if (n == TRDNUM)
        n = 0;
    return n;
}
static void *trd_func(void *p)
{
    int i = (int)p;
    int ch = 'a' + i;

    while (1) {
        pthread_mutex_lock(mut + i);
        write(1, &ch, sizeof(char));
        pthread_mutex_unlock(mut + next(i));
    }
    pthread_exit(NULL);
}

int main(void)
{
    int i, err;
    pthread_t tid[TRDNUM];

    for (i = 0; i < 4; i++) {
        pthread_mutex_init(mut + i, NULL);
        pthread_mutex_lock(mut + i);
        pthread_create(tid + i, NULL, trd_func, (void *)i);
    }
    pthread_mutex_unlock(mut + 0);
    alarm(3);

    for (i = 0; i < 4; i++) {
        pthread_join(tid[i], NULL);
    }
    return 0;
}
