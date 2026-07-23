#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>

#define LEFT    30000000
#define RIGHT   30000200
#define THRNUM  4
/*查询法 的任务池
 * 
 * main线程把人物放入内存中，由其余线程去争夺：争夺方法：抢锁
 * 
 * 优化：使得不会忙等抢锁。
 * */
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static int num = 0;

static void *thr_prime(void *p)
{
    int i;
    int make = 1;
    while (1) {
        make = 1;
        pthread_mutex_lock(&mut);
        while (num == 0) {
            pthread_cond_wait(&cond, &mut);
        }
        if (num == -1) {
            pthread_mutex_unlock(&mut);
            break;
        }

        i = num;
        num = 0;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mut);

        for (int j = 2; j < i / 2; j++) {
            if (i % j == 0) {
                make = 0;
                break;
            }
        }
        if (make)
            printf("thread [%d] %d is primer.\n", (int)p, i);
    }

    pthread_exit(NULL);
    return NULL;
}


int main(void)
{
    pthread_t tid[THRNUM];
    int err, i;

    for (int i = 0; i < THRNUM; i++) {
        err = pthread_create(tid+i, NULL, thr_prime, (void *)(i));
        if (err) {
            fprintf(stderr, "pthread_create fail: %s\n", strerror(err));
            exit(EXIT_FAILURE);
        }
    }

    for (i = LEFT; i <= RIGHT; i++) {
        pthread_mutex_lock(&mut);
        while (num != 0) {
            pthread_cond_wait(&cond, &mut);
        }
        num = i;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mut);
    }

    pthread_mutex_lock(&mut);
    while (num != 0) {
        pthread_cond_wait(&cond, &mut);
    }
    num = -1;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mut);
    printf("111\n");
    for (int i = 0; i < THRNUM; i++) {
        pthread_join(tid[i], NULL);
    }
    return 0;
}
