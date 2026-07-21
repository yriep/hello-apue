#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "mytbf.h"

struct tbf_st
{
    int cps;
    int burst;
    int token;
};
static int inited = 0;
static struct tbf_st * jobs[MAXSIZE_TBF];
static struct sigaction oact;

static void module_unload(void)
{
    struct itimerval itv;
    itv.it_interval.tv_sec  = 0;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 0;
    itv.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &itv, NULL);
    sigaction(SIGALRM, &oact, NULL);

    for (int i = 0; i< MAXSIZE_TBF; i++) {
        free(jobs[i]);
        jobs[i] = NULL;
    }
}

static void sig_action(int sig, siginfo_t *info, void *unused)
{
    if (info->si_code != SI_KERNEL)
        return;

    if (sig == SIGALRM) {
        for (int i = 0; i < MAXSIZE_TBF; i++) {
            if (jobs[i] == NULL)
                continue;

            jobs[i]->token += jobs[i]->cps;
            if (jobs[i]->token > jobs[i]->burst)
                jobs[i]->token = jobs[i]->burst;
        }
    }
}
static void module_load(void)
{
    struct itimerval itv;
    itv.it_interval.tv_sec  = 1;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 1;
    itv.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &itv, NULL);

    struct sigaction act;
    act.sa_sigaction = sig_action;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGALRM, &act, &oact);

    atexit(module_unload);
}
static int get_free_pos(void)
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

    if (!inited)
        module_load();
    inited = 1;

    index = get_free_pos();
    if (index < 0)
        return -ENOSPC;

    jobs[index] = malloc(sizeof(struct tbf_st));
    jobs[index]->cps = cps;
    jobs[index]->burst = burst;
    jobs[index]->token = 0;

    return index;
}

int tbf_destroy(int pos)
{
    free(jobs[pos]);
    jobs[pos] = NULL;
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
    sigset_t set;
    struct tbf_st * tbf = jobs[pos];
    if (tbf == NULL)
        return -EINVAL;

    sigemptyset(&set);
    while (tbf->token <= 0) {
        sigsuspend(&set);
    }
    val = min(token, tbf->token);
    tbf->token -= val;
    return val;
}

int tbf_returtoken(int pos, int token)
{
    struct tbf_st *tbf = jobs[pos];
    if (tbf == NULL)
        return -EINVAL;

    tbf->token += token;
    if (tbf->token > tbf->burst)
        tbf->token = tbf->burst;
    return token;
}
