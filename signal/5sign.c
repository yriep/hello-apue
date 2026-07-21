#include <stdio.h>
#include <signal.h>
#include <unistd.h>

static volatile int lop = 1;

static void sig_handler(int sig)
{
    if (sig == SIGALRM)
        lop = 0;
}

int main(void)
{
    long long cnt = 0;
    alarm(5);
    signal(SIGALRM, sig_handler);

    while (lop) {
        cnt++;
    }
    printf("%lld\n", cnt);
    return 0;
}
