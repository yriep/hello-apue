#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/syslog.h>
#include <unistd.h>
#include <syslog.h>
#include <stdlib.h>

#define FNAME "/tmp/out"

static int daemonize()
{
    int fd;
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        return -1;
    }

    if (pid > 0)        //parent
        exit(0);

    fd = open("/dev/null", O_RDWR);     // 脱离控制终端，将1，2，0设为空设备。
    if (fd < 0) {
        return -1;
    }

    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);

    if (fd > 2)
        close(fd);

    setsid();
    chdir("/");
//      umask(0);
    return 0;
}
int main(void)
{
    FILE *fp;

    openlog("mydaemon", LOG_PID, LOG_DAEMON);
    if (0 != daemonize()) {
        syslog(LOG_ERR, "daemonize() error");
        exit(1);
    }

    syslog(LOG_ERR, "daemonize() success");

    fp = fopen(FNAME, "w");
    if (fp == NULL) {
        syslog(LOG_ERR, "fopen fail %s", strerror(errno));
        exit(1);
    }

    for (int i = 0; ; i++) {
        fprintf(fp, "%d\n", i);
        syslog(LOG_DEBUG, "insert number: %d", i);
        fflush(fp);
        sleep(1);
    }

    fclose(fp);
    closelog();
    return 0;
}
