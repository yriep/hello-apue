#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include "proto.h"


/***
 *  动态进程池
 *  1. 动态创建进程，不是绝对上限和下限，而是对空闲进程进行限制。
 *  2. 同时再对总上限进行限制。
 *
 *  总结：
 *  空闲进程不能少于5，也不能大于10，又总进程数量不能大于20。
 *  空闲进程多了，就杀掉多余进程，空闲进程少了，系统忙碌就再创建新的进程。
 * ***/

#define SIG_NOTIFY      SIGUSR2

#define MINSPAREPROC    5
#define MAXSPAREPROC    10
#define MAXCLIENT       20

enum {STATE_IDEL = 0, STATE_BUSY};

typedef struct serv_st
{
    int status;
    pid_t pid;
    // int reuse;   // record server use count;
} servpool;


static servpool *servpl;
int idle_cnt, busy_cnt;
int sd;

static void serv_job(int slot)
{
    int clnt_sd;
    socklen_t sock_len;
    struct sockaddr_in clnt_addr;
    time_t tm;
    char buf[512];

    sock_len = sizeof(struct sockaddr_in);
    while (1) {
        servpl[slot].status = STATE_IDEL;
        kill(getppid(), SIG_NOTIFY);

        clnt_sd = accept(sd, (void *)&clnt_addr, &sock_len);
        if (0 > clnt_sd) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        servpl[slot].status = STATE_BUSY;
        kill(getppid(), SIG_NOTIFY);

        tm = time(NULL);
        snprintf(buf, 512, FMT_STAMP, (long long)tm);

        if (0 > send(clnt_sd, buf, strlen(buf) + 1, 0)) {
            perror("send");
            exit(EXIT_FAILURE);
        }

        sleep(2);
        close(clnt_sd);
    }
}

static int del_serv(void)
{
    int i = 0;

    if (idle_cnt < MINSPAREPROC) return -1;

    for (i = 0; i < MAXCLIENT; i++) {
        if (servpl[i].status == STATE_IDEL && servpl[i].pid != -1) {
            kill(servpl[i].pid, SIGTERM);
            idle_cnt--;
            servpl[i].pid = -1;
            break;
        }
    }

    return 0;
}

static int add_serv()
{
    int i = 0;
    int csd;
    socklen_t addr_len;
    pid_t pid;

    if (idle_cnt + busy_cnt >= MAXCLIENT) return -1;

    for (i = 0; i < MAXCLIENT; i++) {
        if (servpl[i].pid == -1) break;
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {                  // children
        serv_job(i);
        exit(EXIT_SUCCESS);
    } else {                                //parent
        servpl[i].status = STATE_IDEL;
        servpl[i].pid = pid;
        idle_cnt++;
    }
    return 0;
}

void scan_pool()
{
    int idle = 0, busy = 0;
    for (int i = 0; i < MAXCLIENT; i++) {
        if (servpl[i].pid == -1) continue;

        if (servpl[i].status == STATE_IDEL) {
            idle++;
        } else if (servpl[i].status == STATE_BUSY) {
            busy++;
        } else {
            fprintf(stderr, "unknow");
            abort();
        }
    }
    idle_cnt = idle;
    busy_cnt = busy;
}
static void notify_handler(int sig)
{
    return;
}

int main(void)
{
    struct sockaddr_in serv_addr;
    time_t tm;
    char buf[512];

    struct sigaction act, oact;
    act.sa_handler = SIG_IGN;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NOCLDWAIT;
    sigaction(SIGCHLD, &act, &oact);

    act.sa_handler = notify_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIG_NOTIFY, &act, NULL);

    sigset_t set, oset, save;
    sigaddset(&set, SIG_NOTIFY);
    sigprocmask(SIG_UNBLOCK, &set, &save);
    sigprocmask(SIG_BLOCK, &set, &oset);

    servpl = mmap(NULL, sizeof(servpool) * MAXCLIENT, PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (servpl == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAXCLIENT; i++) {
        servpl[i].pid = -1;
    }

    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (0 > setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(SERV_PORT));
    inet_pton(AF_INET, "0.0.0.0", &serv_addr.sin_addr);
    if (0 > bind(sd, (void *)&serv_addr, sizeof(struct sockaddr_in))) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (0 > listen(sd, 5)) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MINSPAREPROC; i++) {
        add_serv();
    }

    while (1) {
        sigsuspend(&oset);

        scan_pool();
        if (idle_cnt + busy_cnt < MAXCLIENT && idle_cnt < MINSPAREPROC) {
            for (int i = 0; i < MINSPAREPROC - idle_cnt && idle_cnt + busy_cnt < MAXCLIENT; i++) {
                add_serv();
            }
        } else if (idle_cnt > MAXSPAREPROC) {
            for (int i = 0; i < idle_cnt - MAXSPAREPROC; i++) {
                del_serv();
            }
        }

        for (int i = 0; i < MAXCLIENT; i++) {
            if (servpl[i].pid == -1) {
                printf(" ");
            } else {
                printf("%c", (servpl[i].status == STATE_IDEL ? '.' : 'x'));
            }
        }
        putchar('\n');
    }

    sigprocmask(SIG_SETMASK, &save, NULL);

    return 0;
}

