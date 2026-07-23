#include <stdio.h>
#include <pthread.h>
#include <string.h>

static void clean(void *p)
{
    puts((char *)p);
}
static void *func(void *p)
{
    printf("thread start succ\n");

    pthread_cleanup_push(clean, "cleanup: 1");
    pthread_cleanup_push(clean, "cleanup: 2");
    pthread_cleanup_push(clean, "cleanup: 3");
    pthread_cleanup_pop(1);                     //手动清理
    pthread_cleanup_pop(0);                     // 不执行
    pthread_exit(NULL);
    pthread_cleanup_pop(0);                     //exit时自动实现线程清理
}
int main(void)
{
    pthread_t tid;
    int err;

    puts("Begin!");
    err = pthread_create(&tid, NULL, func, NULL);
    if (err) {
        fprintf(stderr, "pthread create: %s\n", strerror(err));
    }

    pthread_join(tid, NULL);
    puts("End!");
    return 0;
}
