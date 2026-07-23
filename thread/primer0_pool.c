#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
/*
** 2026.7.23
** 线程池筛选质数，线程池获取数据时采用查询法。
**/

#define LEFT    30000000
#define RIGHT   30000200
#define THRNUM  4

static int num = 0;
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static void *primer_handler(void *p)
{
    int x = 0;
    int isprimer = 0;

    while (1)
    {
        isprimer = 1;
        pthread_mutex_lock(&mut);
        while (num == 0)
        {
            pthread_cond_wait(&cond, &mut);
        }
        if (num == -1)
        {
            // ! 不需要了，因为设置num=-1时已经广播
            // pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&mut);
            break;
        }
        x = num;
        num = 0;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mut);

        for (int i = 2; i < x / 2; i++)
        {
            if (x % i == 0)
            {
                isprimer = 0;
                break;
            }
        }
        if (isprimer)
        {
            printf("pthread[%d]: [%d] is primer\n", (int)p, x);
        }
    }

    pthread_exit(NULL);
    return NULL;
}

int main(void)
{
    pthread_t tid[THRNUM];

    for (int i = 0; i < THRNUM; i++)
    {
        pthread_create(tid + i, NULL, primer_handler, (void *)i);
    }

    for (int i = LEFT; i <= RIGHT; i++)
    {
        pthread_mutex_lock(&mut);
        while (num != 0) {
            pthread_cond_wait(&cond, &mut);
        }
        num = i;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mut);
    }

    //! 此处为什么一定要加锁呢？
    /*
    ! 不是因为没有 broadcast，而是因为 broadcast 可能发生在工作线程真正进入等待之前，造成丢失唤醒；
    ! 同时 num 的读写没有锁保护，存在数据竞争。broadcast 只能唤醒，不负责同步状态。
    */
    /* 
    ** 1. 没持有锁，就cond_wait是未定义行为
    ** 2. 工作线程a设置num为0后，main线程设置num为-1并广播，而在这间隙中工作线程b，c刚检查完num为0，还没有wait；
    **      那么就会阻塞住。
    */
    pthread_mutex_lock(&mut);
    while (num != 0) {
        pthread_cond_wait(&cond, &mut);
    }
    num = -1;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mut);

    printf("111\n");
    for (int i = 0; i < THRNUM; i++)
    {
        pthread_join(tid[i], NULL);
    }
    return 0;
}