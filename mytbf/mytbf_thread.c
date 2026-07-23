#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include "mytbf.h"
/*
 * 2026.5.27 23:56
 *
 * 视频中的实现，tbf_init返回的是令牌桶结构体。 在我的实现中， 返回的是数组的索引
 *
 * 视频中对于fetch 和 return token 没有加mut_job；
 * 我返回的是job数组的索引，需要查找jobs；
 * 那么是否要加mutjob呢？
 * 
 * 这取决于处理job时会不会有竞争。
 * 1. 调用令牌桶的模块是单线程时对令牌桶数组的处理，都不会有竞争，单线程按序执行，不会有竞争。只有令牌桶的实现中，
 * 对token的处理导致可能的竞争。
 *
 * 2. 多线程处理同一个令牌
 * 肯定会有竞争的发生。但是对于fetch和token 不加锁能避免竞争的发生吗？
 * 不可以，很有可能一个线程要destroy，一个还在fetch。
 *
 * 那么就算是视频中的情况，其实他也需要加锁，或者是多线程使用tbf时加锁更好！
 * 没错，应该是由使用者限制的。不同线程的锁 lock or unlock is undefine 是未定义的。
 *
 * 2026.5.28 --> 最终发现一个问题，fetch加mutjob，会导致死锁。
 * */

struct tbf_st
{
    int cps;
    int burst;
    int token;
    pthread_mutex_t mut;
    pthread_cond_t cond;
};

static pthread_t tid;
static struct tbf_st * jobs[MAXSIZE_TBF];
static pthread_mutex_t mut_job = PTHREAD_MUTEX_INITIALIZER;
static pthread_once_t one = PTHREAD_ONCE_INIT;

static void module_unload(void)
{
    pthread_cancel(tid);
    pthread_join(tid, NULL);
    for (int i = 0; i< MAXSIZE_TBF; i++) {
        if (jobs[i] == NULL) {
            continue;
        }
        // tbf_destroy()
        pthread_mutex_destroy(&(jobs[i]->mut));
        pthread_cond_destroy(&(jobs[i]->cond));
        free(jobs[i]);
        jobs[i] = NULL;
    }

    pthread_mutex_destroy(&mut_job);
}

static void *token_comp_start(void *p)
{
    while (1) {
        pthread_mutex_lock(&mut_job);
        for (int i = 0; i < MAXSIZE_TBF; i++) {
            if (jobs[i] == NULL)
                continue;

            pthread_mutex_lock(&jobs[i]->mut);
            jobs[i]->token += jobs[i]->cps;
            if (jobs[i]->token > jobs[i]->burst)
                jobs[i]->token = jobs[i]->burst;
            pthread_cond_broadcast(&jobs[i]->cond);
            pthread_mutex_unlock(&jobs[i]->mut);
        }
        pthread_mutex_unlock(&mut_job);
        sleep(1);
    }
}

static void module_load(void)
{
    int err;

    err = pthread_create(&tid, NULL, token_comp_start, NULL);
    if (err) {
        fprintf(stderr, "pthread_create: %s\n", strerror(err));
        exit(EXIT_FAILURE);
    }
    atexit(module_unload);
}

//函数名体现unlock，防止误调用
static int get_free_pos_unlock(void)
{
    int i;
    for (i = 0; i < MAXSIZE_TBF; i++) {
        if (jobs[i] == NULL)
            return i;
    }
    return -1;
}

int mytbf_init(int cps, int burst)
{
    int index;
    pthread_once(&one, module_load);

    struct tbf_st *tbf;
    tbf = malloc(sizeof(struct tbf_st));
    tbf->cps = cps;
    tbf->burst = burst;
    tbf->token = 0;
    pthread_mutex_init(&tbf->mut, NULL);
    pthread_cond_init(&tbf->cond, NULL);

    pthread_mutex_lock(&mut_job);
    index = get_free_pos_unlock();
    if (index < 0)
        return -ENOSPC;
    jobs[index] = tbf;
    pthread_mutex_unlock(&mut_job);
    return index;
}

int tbf_destroy(int pos)
{
    if (jobs[pos] == NULL) {
        return EINVAL;
    }
    pthread_mutex_lock(&mut_job);
    pthread_mutex_destroy(&jobs[pos]->mut);
    pthread_cond_destroy(&jobs[pos]->cond);
    free(jobs[pos]);
    jobs[pos] = NULL;
    pthread_mutex_unlock(&mut_job);

    return 0;
}

static int min(int a, int b)
{
    if (a < b)
        return a;
    return b;
}

int tbf_fetchtoken(int pos, int token)
{
    int val;

    struct tbf_st * tbf = jobs[pos];
    if (tbf == NULL)
        return -EINVAL;

    pthread_mutex_lock(&tbf->mut);
    while (tbf->token <= 0) {
        pthread_cond_wait(&tbf->cond, &tbf->mut);
    }
    val = min(token, tbf->token);
    tbf->token -= val;
    pthread_mutex_unlock(&tbf->mut);

    return val;
}

int tbf_returtoken(int pos, int token)
{
    struct tbf_st *tbf = jobs[pos];
    if (tbf == NULL)
        return -EINVAL;

    pthread_mutex_lock(&tbf->mut);
    tbf->token += token;
    if (tbf->token > tbf->burst)
        tbf->token = tbf->burst;
    pthread_cond_broadcast(&tbf->cond);
    pthread_mutex_unlock(&tbf->mut);

    return token;
}
