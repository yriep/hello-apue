#include <stdio.h>
#include <unistd.h>
#include "anytimer.h"

void f1(void *c)
{
    printf("%s", (char *)c);
    fflush(stdout);
}

void f2(void *c)
{
    printf("%s", (char *)c);
    fflush(stdout);
}
int main(void)
{
    puts("begin!");
    /* 
     * 5 f1 aaa
     * 2 f2 bbb
     * 7 f1 ccc
     *
     * printf: beginEnd!...bbb..aaa...ccc....
     * */
    int a, b, c;
    a = at_addjob(5, f1, "aaa");
    b = at_addjob(3, f2, "bbb");
    c = at_addjob(8, f1, "ccc");

    at_canceljob(b);
    puts("End!");
    while (1) {
        printf(".");
        fflush(stdout);
        sleep(1);
    }
    at_waitjob(a);
    at_waitjob(b);
    at_waitjob(c);
    return 0;
}
