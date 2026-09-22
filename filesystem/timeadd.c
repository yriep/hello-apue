#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*  当前时间加上100天后是什么适合。*/

int main(void)
{
    time_t tm;
    struct tm *tm_s;
    char buf[1024];

    tm = time(NULL);
    if ((time_t)-1 == tm) {
        fprintf(stderr, "time error\n");
        exit(EXIT_FAILURE);
    }

    tm_s = localtime(&tm);
    if (tm_s == NULL) {
        perror("localtime");
        exit(EXIT_FAILURE);
    }

    strftime(buf, 1024, "%Y-%m-%d %H-%M-%S", tm_s);
    puts(buf);

    tm_s->tm_mday += 100;
    tm = mktime(tm_s);
    if ((time_t)-1 == tm) {
        perror("mktime");
        exit(EXIT_FAILURE);
    }

    strftime(buf, 1024, "+100 day time: %Y-%m-%d %H-%M-%S", tm_s);
    puts(buf);

    return 0;
}
