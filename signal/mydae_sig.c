#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#define FNAME "/tmp/out"

FILE *fp;
/*
 * 创建一个deamon进程，一直向临时文件中写入数字
 * deamon 进程就是要为一个会话的leader 一个组的leader。 而使用setsid正好能达到这个功能。又setsid不能是一个进程组的leader，
 * 先使用fork保证不是leader
 
 ! daemon 的优化
 * 优化：释放守护进程的资源，close fp and closelog。
 * 使用signal和sigaction。由于可能会有多种信号都会结束进程，信号处理函数会重入，为了阻塞其余进程，
 * 使用sigaction，处理一个进程时阻塞其余信号。
 */
static int deamonrize()
{
    pid_t pid;
    int fd;

    pid = fork();
    if (pid < 0) {
        return -1;
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    printf("get ppid: %d\n",getppid());
    setsid();
    fd = open("/dev/null", O_RDWR);
    if (fd < 0) {
        return -1;
    }
    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);

    if (fd > 2)
        close(fd);

    chdir("/");
    return 0;
}
static void sig_handler(int sig)
{
    syslog(LOG_ERR, "deamon quit");
    closelog();
    fclose(fp);
    exit(0);
}
int main(void)
{
    struct sigaction act;

    act.sa_handler = sig_handler;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGINT | SIGTERM | SIGQUIT);
    act.sa_flags = 0;
    //优化，能够释放资源，使用信号机制；使用signal时，sig_handler有重入风险，故使用sigaction
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);

    openlog("mydeamon", LOG_PID, LOG_DAEMON);
    if (0 > deamonrize()) {
        syslog(LOG_ERR, "deamonrize() failed");
        exit(EXIT_FAILURE);
    }

    syslog(LOG_ERR, "deamonrize() success!");

    fp = fopen(FNAME, "a");
    if (fp == NULL) {
        syslog(LOG_ERR, "fopen failed: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    for (int i = 0;  ; i++) {
        fprintf(fp, "%d\n", i);
        syslog(LOG_INFO, "input %d", i);
        fflush(fp);
        sleep(1);
    }
    return 0;
}
