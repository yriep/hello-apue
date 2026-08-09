#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define     DEV1    "/dev/tty10"
#define     DEV2    "/dev/tty11"
#define     BUFSIZE 1024
/*
 * 使用有限状态机实现数据中继(对两个设备进行数据交换)
 */
enum {
    STAT_R = 1,
    STAT_W,
STAT_AUTO,
    STAT_Ex,
    STAT_T,
};

typedef struct
{
    int status;
    int sfd;
    int dfd;
    char buf[BUFSIZE];
    int len;
    int pos;
    char *errmsg;
} STAT_ST;

static void state_drive(STAT_ST *stat)
{
    ssize_t len;
    switch (stat->status) {
        case STAT_R:
            stat->pos = 0;
            stat->len = read(stat->sfd, stat->buf, BUFSIZE - 1);
            if (stat->len < 0) {
                if (errno == EAGAIN)
                    stat->status = STAT_R;
                else {
                    stat->errmsg = "read()";
                    stat->status = STAT_Ex;
                }
            } else if (stat->len == 0) {
                stat->status = STAT_T;
            } else {
                stat->status = STAT_W;
            }
            break;
        case STAT_W:
            len = write(stat->dfd, stat->buf + stat->pos, stat->len);
            if (len < 0) {
                if (errno == EAGAIN)
                    stat->status= STAT_W;
                else {
                    stat->errmsg = "write()";
                    stat->status = STAT_Ex;
                }
            } else {
                stat->pos += len;
                stat->len -= len;
                if (stat->len == 0)
                    stat->status = STAT_R;
                else
                    stat->status = STAT_W;
            }
            break;
        case STAT_Ex:
            fprintf(stderr, "%s\n", stat->errmsg);
            stat->status = STAT_T;
            break;
        case STAT_T:
            break;
        default:
            abort();
            break;
    }
}
static int max(int a, int b)
{
    return (a > b ? a : b);
}
static void relay(int fd1, int fd2)
{
    int save_fd1, save_fd2;
    STAT_ST stat12, stat21;
    fd_set r_fdset, w_fdset, r_tmpset, w_tmpset;
    int fd_cnt = 0;

    save_fd1 = fcntl(fd1, F_GETFL);
    fcntl(fd1, F_SETFL, save_fd1 | O_NONBLOCK);

    save_fd2 = fcntl(fd2, F_GETFL);
    fcntl(fd2, F_SETFL, save_fd2 | O_NONBLOCK);

    memset(&stat12, 0, sizeof(STAT_ST));
    memset(&stat21, 0, sizeof(STAT_ST));
    stat12.sfd = fd1;
    stat12.dfd = fd2;
    stat12.status = STAT_R;

    stat21.sfd = fd2;
    stat21.dfd = fd1;
    stat21.status = STAT_R;

    /*
     * 使用select监视套接字，防止盲等。
     *      布置监视任务
     *      监视
     *      查看监视结果
     **/

    /*
     *
     * 我觉得直接监听两个就很好。简单明了
     */
    FD_ZERO(&r_fdset);
    FD_SET(fd1, &r_fdset);
    FD_SET(fd2, &r_fdset);

    FD_ZERO(&w_fdset);
    FD_SET(fd1, &w_fdset);
    FD_SET(fd2, &w_fdset);

    while (stat12.status != STAT_T || stat21.status != STAT_T) {
        /*
         李慧芹写法
         当状态机stat12为w时，write（fd2）；此时若向fd1写入，select同样会监测到fd1的行为为IN；但此时执行状态机为w是有问题的，无法执行read（fd1）。
         1. 但实际上是执行不到的，因为循环还没到select函数，并没有相应的反应。 所以我这样写是可以的。
         2. 是能执行到的，当一次write没有写完时，那么第二循环select就同时有fd1的read和fd2的write行为，而此时状态机在w状态，那就无法执行read。
         但是这种情况下，若区分状态机状态，都监视不到这种行为，更是无法处理。如果不区分，让状态机自己转，就是write写完后，状态转到r，再执行read
            而select默认条件触发，缓冲区有数据，会一直通知。
        FD_ZERO(&r_tmpset);
        FD_ZERO(&w_tmpset);
        if (stat12.status == STAT_R)
            FD_SET(stat12.sfd, &r_tmpset);
        if (stat12.status == STAT_W)
            FD_SET(stat12.dfd, &w_tmpset);
        if (stat21.status == STAT_R)
            FD_SET(stat21.sfd, &r_tmpset);
        if (stat21.status == STAT_W)
            FD_SET(stat21.dfd, &w_tmpset);
        */
        r_tmpset = r_fdset;
        w_tmpset = w_fdset;

        if (stat12.status < STAT_AUTO || stat21.status < STAT_AUTO) {
            fd_cnt = select(max(fd1, fd2)+1, &r_tmpset, &w_tmpset, NULL, NULL);
            if (fd_cnt < 0) {
                if (errno == EINTR)
                    continue;
                perror("select()");
                exit(EXIT_FAILURE);
            }
        }
        if (FD_ISSET(stat12.sfd, &r_tmpset) || FD_ISSET(stat12.dfd, &w_tmpset) || stat12.status > STAT_AUTO)
            state_drive(&stat12);
        if (FD_ISSET(stat21.sfd, &r_tmpset) || FD_ISSET(stat21.dfd, &w_tmpset) || stat21.status > STAT_AUTO)
            state_drive(&stat21);
    }
    fcntl(fd1, F_SETFL, save_fd1);
    fcntl(fd2, F_SETFL, save_fd2);
}
int main(void)
{
    int fd1, fd2;
    fd1 = open(DEV1, O_RDWR);
    if (fd1 < 0) {
        fprintf(stderr, "open(): %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    write(fd1, "tty1\n", 5);

    fd2 = open(DEV2, O_RDWR);
    if (fd2 < 2) {
        fprintf(stderr, "open(): %s\n", strerror(errno));
        close(fd1);
        exit(EXIT_FAILURE);
    }
    write(fd2, "tty2\n", 5);

    relay(fd1, fd2);

    close(fd1);
    close(fd2);
    return 0;
}
