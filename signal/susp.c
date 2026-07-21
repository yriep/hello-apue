#include <stdatomic.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
/*
 * 1. 10hang 5lie *
 *
 * 忽视每一行打印时的ctrlc，直到每一行打印完成，新起一行时，打出～
 * 所以要在每一行打印时阻塞信号，到新一行放开，pause，拿到信号后就继续
 * 又因为pause操作不原子，改成sigsuspend。
 *
 * 新加修改：使用实时信号
 * 实时信号的特点：按时传输，而标准信号会丢失，传输顺序 未定义
 * 那么按照定义，我一次性输入5条实时信号，也会依次处理又sigsuspend是原子操作，那么就会打印五行。
 */

void sig_handler(int sig)
{
    if (sig == SIGRTMIN+6)
        write(1, "~", sizeof(char));
}

int main(void)
{
    sigset_t set, oset, save;
    struct sigaction act;

    act.sa_handler = sig_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGRTMIN+6, &act, NULL);

    sigemptyset(&set);
    sigaddset(&set, SIGRTMIN+6);
    sigprocmask(SIG_UNBLOCK, &set, &save);
    sigprocmask(SIG_BLOCK, &set, &oset);

    for (int i = 0; i < 10; i++) {
        for (int j = 0 ; j < 5; j++) {
            write(1, "*", sizeof(char));
            sleep(1);
        }
        write(1, "\n", sizeof(char));
        sigsuspend(&oset);
    }
    sigprocmask(SIG_SETMASK, &save, NULL);
    return 0;
}
