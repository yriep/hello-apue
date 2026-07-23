#include <stdio.h>
#include <signal.h>
#include <unistd.h>
/*
sigsuspend
信号驱动程序。

 * 打印5行5星，每到新的一行要等待siging信号，才会打印。

 * 1. 打印一行时阻塞sigint信号，不会插入！；直到新的一行时再打印！
 * 2. 阻塞sigint信号，使用pause等待信号到来，按理说没有sigint时会阻塞，有就会直接打印下一行
 *      但是操作不原子，有可能放开阻塞，还没调用pause时，就处理完信号了，
 *      然后调用pause又要等待下一个信号接收。
 */
static void sig_handler(int s)
{
   write(1, "!", 1);
}

static void sig_action(int s, const siginfo_t *info, void *unused)
{
    if (info->si_code == SI_USER)
        return;
    write(1, "~", 1);
}

int main(void)
{
    sigset_t set, save, oset;

    signal(SIGINT, sig_handler);

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigprocmask(SIG_UNBLOCK, &set, &save);
    // ! it's necessary to record unblock set, because of old set is unknow.
    // ! it's possible that SIGINT is block and can't handler function.
    sigprocmask(SIG_BLOCK, &set, &oset);

    for (int j = 0; j < 5; j++) {
        for (int i = 0; i < 5; i++) {
            write(1, "*", sizeof(char));
            sleep(1);
        }
        write(1, "\n", 1);
        /*
        ! sigsuspend 等价于下面三个函数的原子操作。
        ! 先解除阻塞，等待调用pause，调用完之后在阻塞住。继续打印下一行。
        sigset_t tmp;
        sigprocmask(SIG_SETMASK, &oset, &tmp);      //oset to unblock, tmp to block
        pause()
        sigprocmask(SIG_SETMASK, &tmp, NULL);
        */
        sigsuspend(&oset);
    }
    sigprocmask(SIG_SETMASK, &save, NULL);
    return 0;
}
