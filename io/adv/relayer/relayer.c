#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include "relayer.h"

enum
{
    STAT_R = 1,
    STAT_W,
STAT_AUTO,
    STAT_Ex,
    STAT_T,
};

typedef struct file_machina
{
    int status;
    int s_fd;
    int d_fd;
    char *buf;
    int len;
    int pos;
    char *errstr;
    int64_t counts;
} fm_t;

typedef struct relay_job_st
{
    fm_t * fm1, *fm2;
    int fd1, fd2;
    int status;
    int fd1_save;
    int fd2_save;
} rel_job_st;

static rel_job_st *JOB[MAX_JOB];

static pthread_once_t once = PTHREAD_ONCE_INIT;
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

static void state_drive(fm_t *stat)
{
    ssize_t len;
    switch (stat->status)
    {
        case STAT_R:
            stat->pos = 0;
            stat->counts++;
            stat->len = read(stat->s_fd, stat->buf, BUFSIZE - 1);
            if (stat->len < 0) {
                if (errno == EAGAIN)
                    stat->status = STAT_R;
                else {
                    stat->errstr = "read()";
                    stat->status = STAT_Ex;
                }
            } else if (stat->len == 0) {
                stat->status = STAT_T;
            } else {
                stat->status = STAT_W;
            }
            break;

        case STAT_W:
            len = write(stat->d_fd, stat->buf + stat->pos, stat->len);
            if (len < 0) {
                if (errno == EAGAIN)
                    stat->status= STAT_W;
                else {
                    stat->errstr = "write()";
                    stat->status = STAT_Ex;
                }
            } else {
                stat->pos += len;
                stat->len -= len;
                if (stat->len == 0)
                    stat->status = STAT_R;
                else
                    stat->status = STAT_W;
            }
            break;

        case STAT_Ex:
            fprintf(stderr, "%s\n", stat->errstr);
            stat->status = STAT_T;
            break;

        case STAT_T:
            break;

        default:
            abort();
            break;
    }
}

static int get_free_pos_unlock()
{
    for (int i = 0; i < MAX_JOB; i++) {
        if (JOB[i] == NULL)
            return i;
    }
    return -1;
}

static void module_unload(void)
{
}

static void *relay_drive(void *argv)
{
    while (1) {
        pthread_mutex_lock(&mut);
        for (int i = 0; i < MAX_JOB; i++) {
            if (JOB[i] == NULL)
                continue;

            if (JOB[i]->status == RUNNING) {
                state_drive(JOB[i]->fm1);
                state_drive(JOB[i]->fm2);

                if (JOB[i]->fm1->status == STAT_T || JOB[i]->fm2->status == STAT_T) {
                    JOB[i]->status = STOPED;
                }
            }
        }
        pthread_mutex_unlock(&mut);
    }
    return NULL;
}

static void module_load(void)
{
    pthread_t pid;
    int err;

    err = pthread_create(&pid, NULL, relay_drive, NULL);
    if (err < 0) {
        perror("pthread_create()");
        exit(EXIT_FAILURE);
    }
    atexit(module_unload);
}

int relay_addjob(int fd1, int fd2)
{
    int pos;
    fm_t *fm1, *fm2;
    rel_job_st *job;

    pthread_once(&once, module_load);

    job = malloc(sizeof(rel_job_st));
    job->fd1 = fd1;
    job->fd2 = fd2;
    job->status = RUNNING;

    job->fd1_save = fcntl(fd1, F_GETFL);
    fcntl(fd1, F_SETFL, job->fd1_save | O_NONBLOCK);
    job->fd2_save = fcntl(fd2, F_GETFL);
    fcntl(fd2, F_SETFL, job->fd2_save | O_NONBLOCK);

    fm1 = malloc(sizeof(fm_t));
    memset(fm1, 0, sizeof(fm_t));
    fm1->s_fd = fd1;
    fm1->d_fd = fd2;
    fm1->status = STAT_R;
    fm1->buf = calloc(sizeof(char), BUFSIZE);
    job->fm1 = fm1;

    fm2 = malloc(sizeof(fm_t));
    memset(fm2, 0, sizeof(fm_t));
    fm2->s_fd = fd2;
    fm2->d_fd = fd1;
    fm2->status = STAT_R;
    fm2->buf = calloc(sizeof(char), BUFSIZE);
    job->fm2 = fm2;

    pthread_mutex_lock(&mut);
    pos = get_free_pos_unlock();
    if (pos < 0) {
        pthread_mutex_unlock(&mut);
        fcntl(fd1, F_SETFL, job->fd1_save);
        fcntl(fd2, F_SETFL, job->fd2_save);
        free(fm1->buf);
        free(fm2->buf);
        free(fm1);
        free(fm2);
        free(job);
        return ENOSPC;
    }
    JOB[pos] = job;
    pthread_mutex_unlock(&mut);

    return pos;
}


int relay_canceljob(int fm)
{
    return 0;
}


int relay_waitjob(int fm)
{
    return 0;
}

