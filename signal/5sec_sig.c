#include <stdio.h>
#include <signal.h>
#include <unistd.h>

static int loop = 1;

static void sig_handler(int sig)
{
    if (sig == SIGALRM) {
        loop = 0;
    }
}

int main(void)
{
    long long count = 0;
    alarm(5);
    signal(SIGALRM, sig_handler);

    time_t end = time(NULL) + 5;

    /*
    while (time(NULL) <= end)
    {
        count++;
    }
    */

    while (loop)
    {
        count++;
    }

    printf("%lld\n", count);
    return 0;
}
