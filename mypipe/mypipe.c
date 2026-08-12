#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "mypipe.h"

struct pipe_st
{
    int head;
    int tail;
    int datesize;
    char date[MAX_PIPE];
    int  rd_cnt;
    int  wr_cnt;
    pthread_mutex_t mut;
    pthread_cond_t cond;
};

pipe *pipe_init(void)
{
    struct pipe_st *pt;

    pt = malloc(sizeof(struct pipe_st));
    if (pt == NULL) {
        return NULL;
    }

    pt->head = 0;
    pt->tail = 0;
    pt->datesize = 0;
    pthread_mutex_init(&pt->mut, NULL);
    pthread_cond_init(&pt->cond, NULL);
    return pt;
}

int pipe_destroy(pipe *p)
{
    struct pipe_st *pt = p;
    pthread_mutex_destroy(&pt->mut);
    pthread_cond_destroy(&pt->cond);
    free(pt);
    return 0;
}

int pipe_register(pipe *p, int opt)
{
    struct pipe_st *pt = p;

    pthread_mutex_lock(&pt->mut);
    if (opt & READER) {
        pt->rd_cnt++;
    }

    if (opt & WRITER) {
        pt->wr_cnt++;
    }

    pthread_cond_broadcast(&pt->cond);
    // if (pt->rd_cnt == 0 || pt->wr_cnt == 0) {    需要循环等待。
    while (pt->rd_cnt <= 0 || pt->wr_cnt <= 0) {
        pthread_cond_wait(&pt->cond, &pt->mut);
    }

    pthread_mutex_unlock(&pt->mut);
    return 0;
}

int pipe_unregister(pipe *p, int opt)
{
    struct pipe_st *pt = p;

    pthread_mutex_lock(&pt->mut);
    if (opt & READER) {
        pt->rd_cnt--;
    }

    if (opt & WRITER) {
        pt->wr_cnt--;
    }
    pthread_mutex_unlock(&pt->mut);
    return 0;
}
static int next(int i)
{
    if (i + 1 < MAX_PIPE) {
        return i+1;
    } else {
        return 0;
    }
}
static int pipe_readbyte(struct pipe_st *pt, void *buf)
{
    if (pt->datesize <= 0) {
        return -1;
    }
    char *ch = buf;
    *ch = pt->date[pt->head];
    pt->head = next(pt->head);
    pt->datesize--;
    return 0;
}

int pipe_read(pipe *p, void *buf, int size)
{
    int i = 0;
    struct pipe_st *pt = p;

    pthread_mutex_lock(&pt->mut);
    while (pt->datesize <= 0 && pt->wr_cnt > 0) {
        pthread_cond_wait(&pt->cond, &pt->mut);
    }

    if (pt->datesize <= 0 && pt->wr_cnt <= 0) {
        pthread_mutex_unlock(&pt->mut);
        return -1;
    }

    for (i = 0; i < size; i++) {
        if (pipe_readbyte(pt, buf + i) != 0) {
            break;
        }
    }
    pthread_cond_broadcast(&pt->cond);
    pthread_mutex_unlock(&pt->mut);
    return i;
}
static int pipe_writebyte(struct pipe_st *pt, const void *buf)
{
    if (pt->datesize >= MAX_PIPE)
        return -1;
    const char *ch = buf;
    pt->date[pt->tail] = *ch;
    pt->tail = next(pt->tail);
    pt->datesize++;
    return 0;
}
int pipe_write(pipe *p, const void *buf, int size)
{
    int i;
    struct pipe_st *pt = p;

    pthread_mutex_lock(&pt->mut);
    while (pt->datesize >= MAX_PIPE && pt->rd_cnt > 0) {
        pthread_cond_wait(&pt->cond, &pt->mut);
    }

    if (pt->rd_cnt <= 0) {
        pthread_mutex_unlock(&pt->mut);
        return -1;
    }
    for (i = 0; i < size; i++) {
        if (pipe_writebyte(pt, buf+i) < 0) {
            break;
        }
    }
    pthread_cond_broadcast(&pt->cond);
    pthread_mutex_unlock(&pt->mut);
    return i;
}
