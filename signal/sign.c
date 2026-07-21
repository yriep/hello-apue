#include <stdio.h>
#include <signal.h>
#include <unistd.h>
/*
 * 打印5行5星
 * 1. 打印一行时阻塞sigint信号，不会插入！；直到新的一行时再打印！
 * 2. 阻塞sigint信号，使用pause等待信号到来，按理说没有sigint时会阻塞，有就会直接打印下一行
 *      但是操作不原子，有可能放开阻塞，还没调用pause时，就处理完信号了，然后调用pause又要等待下一个信号接收。
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
    sigset_t set, save, old;
    struct sigaction act;

    act.sa_sigaction = sig_action;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &act, NULL);

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigprocmask(SIG_UNBLOCK, &set, &save);
    sigprocmask(SIG_BLOCK, &set, &old);

    for (int j = 0; j < 5; j++) {
        //sigprocmask(SIG_BLOCK, &set, &old);
        for (int i = 0; i < 5; i++) {
            write(1, "*", sizeof(char));
            sleep(1);
        }
        write(1, "\n", 1);

        sigsuspend(&old);
    }
    sigprocmask(SIG_SETMASK, &save, NULL);
    return 0;
}
        /*
         *
        signal(SIGINT, sig_handler);
        signal(SIGINT, SIG_IGN);

        sigprocmask(SIG_UNBLOCK, &set, NULL);
        pause();
        sigprocmask(SIG_BLOCK, &set, NULL);
        */
        /*
        sigprocmask(SIG_UNBLOCK, &set, &save);
        sigprocmask(SIG_BLOCK, &set, &oset);        // it's necessary to record unblock set, because of old set is unknow.
        // *等价于下面三个函数的原子操作。
        // 1. 先解除阻塞，等待调用pause，调用完之后在阻塞住。继续打印下一行。
         sigset_t tmp;
         sigprocmask(SIG_SETMASK, &oset, &tmp);      //oset to unblock, tmp to block
         pause()
         sigprocmask(SIG_SETMASK, &tmp, NULL);
         * */
