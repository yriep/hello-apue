#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include "anytimer.h"

enum job_status {running, terminate, finish};

typedef struct anytimer_st
{
    int sec;
    at_jobfunc_t func;
    void *argv;
    int flag;
} st_timer;

static st_timer * job[MAX_TIMER];
static sig_t save;
static _Atomic int inited = 0;

void alrm_handler(int sig)
{
    if (sig == SIGALRM) {
        // putchar('x');
        alarm(1);
        for (int i = 0; i < MAX_TIMER; i++) {
            if (job[i] != NULL && job[i]->flag == running) {
                job[i]->sec--;

                if (job[i]->sec == 0) {
                    job[i]->func(job[i]->argv);
                    job[i]->flag = finish;
                }
            }
        }
    }
    
}

static void module_unload(void)
{
    alarm(0);
    signal(SIGALRM, save);
    for (int i = 0; i < MAX_TIMER; i++) {
        free(job[i]);
    }
}

static void module_load(void)
{
    alarm(1);
    sig_t save = signal(SIGALRM, alrm_handler);
    atexit(module_unload);
}

static int get_free_index()
{
    int index = 0;
    for (index = 0; index < MAX_TIMER; index++) {
        if (job[index] == NULL)
            return index;
    }
    return -1;
}

int at_addjob(int sec, at_jobfunc_t jobp, void *argv)
{
    int index;
    st_timer *timer = NULL;

    if (!inited)
        module_load();
    inited = 1;

    timer = malloc(sizeof(st_timer));
    memset(timer, 0, sizeof(st_timer));

    timer->sec = sec;
    timer->func = jobp;
    timer->argv = argv;
    timer->flag = running;

    index = get_free_index();
    if (index < 0)
        return -ENOSPC;

    job[index] = timer;
    return index;
}


int at_canceljob(int id)
{
    if (job[id] == NULL)
        return -EINVAL;

    job[id]->flag = terminate;
    return 0;
}


int at_waitjob(int id)
{
    if (job[id] == NULL)
        return -EINVAL;

    while (job[id]->flag != finish && job[id]->flag != terminate) {
        pause();
    }

    free(job[id]);
    job[id] = NULL;
    return 0;
}

