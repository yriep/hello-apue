#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define LEFT    30000000
#define RIGHT   30000200
#define THRNUM  RIGHT - LEFT + 1

static void *thr_prime(void *p)
{
    int i;
    int make = 1;
    i = *(int *)p;

    for (int j = 2; j < i / 2; j++) {
        if (i % j == 0) {
            make = 0;
            break;
        }
    }
    if (make)
        printf("%d is primer.\n", i);
    pthread_exit(NULL);
    return NULL;
}


int main(void)
{
    pthread_t tid[THRNUM];
    int err;
    int p[THRNUM];

    for (int i = LEFT; i <= RIGHT; i++) {
        p[i-LEFT] = i;
        // 如果参数直接传i地址，会有竞争出现。
        // 传递的是i的地址，每次循环i值改变，那么线程循环判断时，判断的就是改变后的i
        /*
         * 解决方法 
         * 1. 直接将i强转成指针 (void *)i; 会有警告
         * 2. 捏造一个结构体 primer0e.c
        */
        err = pthread_create(tid+i-LEFT, NULL, thr_prime, (void *)(p+i-LEFT));
        if (err) {
            fprintf(stderr, "pthread_create fail: %s\n", strerror(err));
            exit(EXIT_FAILURE);
        }
    }

    for (int i = LEFT; i <= RIGHT; i++) {
        pthread_join(tid[i-LEFT], NULL);
    }
    return 0;
}
