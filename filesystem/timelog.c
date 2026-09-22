#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#define FPATH   "/tmp/out"
#define BUFSIZE 1024


int main(void)
{
    int fd;
    int cnt = 0 ;
    time_t tm;
    struct tm *tm_s;
    char buf[BUFSIZE];

    fd = open(FPATH, O_CREAT | O_RDWR | O_APPEND, 0600);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    FILE *fp = fdopen(fd, "a+");
    while (NULL != fgets(buf, BUFSIZE, fp)) {
        cnt++;
    }

    while (1) {
        tm = time(NULL);
        tm_s = localtime(&tm);
        snprintf(buf, BUFSIZE, "%-4d %4d-%2d-%2d: %02d-%02d-%02d\n", ++cnt, \
                tm_s->tm_year+1900, tm_s->tm_mon+1, tm_s->tm_mday, \
                tm_s->tm_hour, tm_s->tm_min, tm_s->tm_sec);
        write(fd, buf, strlen(buf));
        sleep(1);
    }

    fclose(fp);
    return 0;
}
