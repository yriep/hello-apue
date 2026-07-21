#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "mytbf.h"

/*
 * tbf 令牌桶 按照token速度进行传输
 * 1. 流量 cps，2 token = cps 3 最大容量 4 保存位置
 *
 * 2. 1 获取token 2. 返回token。
 *
 *
 * 优化：使用setitimer和sigaction
 * 1. setitimer优势：周期性调用，时间灵活
 * 2. sigaction优势区分信号来源，防止user使用kill发送sigalrm信号导致token快速积累。
 * */
typedef void (*sig_t) (int);

struct mytbf_st {
    int cps;
    int burst;
    int token;
    int pos;
};
static int inited = 0;
static struct mytbf_st *job[MAX_TBF_SIZE];
static sig_t save_handler;

static void alrm_handler(int sig)
{
    int i = 0;
    alarm(1);
    for (i = 0; i < MAX_TBF_SIZE; i++) {
        if (job[i] != NULL) {
            job[i]->token += job[i]->cps;
            if (job[i]->token > job[i]->burst)
                job[i]->token = job[i]->burst;
        }
    }
}

static void module_unload(void)
{
    alarm(0);
    signal(SIGALRM, save_handler);
    for (int i = 0; i < MAX_TBF_SIZE; i++) {
        free(job[i]);
    }
}
static void module_load(void)
{
    alarm(1);
    save_handler = signal(SIGALRM, alrm_handler);
    atexit(module_unload);
}

static int get_free_pos(void)
{
    int i = 0;
    for (i = 0; i < MAX_TBF_SIZE; i++)
    {
        if (job[i] == NULL)
            return i;
    }

    return -1;
}
mytbf_t *mytbf_init(int cps, int burst)
{
    struct mytbf_st *mtbf;
    int pos = 0;

    if (!inited) {
        module_load();
        inited = 1;
    }

    mtbf = malloc(sizeof(struct mytbf_st));
    memset(mtbf, 0, sizeof(struct mytbf_st));

    pos = get_free_pos();
    if (pos < 0)
        return NULL;

    mtbf->cps = cps;
    mtbf->burst = burst;
    mtbf->token = 0;
    mtbf->pos = pos;
    job[pos] = mtbf;
    return mtbf;
}


static int min(int a, int b)
{
    if (a < b)
        return a;
    return b;
}
int tbftoken_fetch(mytbf_t *tbf, int size)
{
    struct mytbf_st *me = tbf;
    if (size <= 0)
        return -EINVAL;

    while (me->token <= 0)
        pause();

    int token = min(me->token, size);
    me->token -= token;
    return token;
}


int tbftoken_retur(mytbf_t *tbf, int size)
{
    struct mytbf_st *me = tbf;
    if (size <= 0)
        return -EINVAL;

    me->token += size;
    if (me->token > me->burst)
        me->token = me->burst;
    return size;
}


int tbf_destroy(mytbf_t *tbf_t)
{
    struct mytbf_st *ptr = tbf_t;
    job[ptr->pos] = NULL;
    free(tbf_t);
    return 0;
}
