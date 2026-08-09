#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>

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
static void relay(int fd1, int fd2)
{
    int save_fd1, save_fd2;
    STAT_ST stat12, stat21;
    struct epoll_event event, *events;
    int epfd;

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

     /* epoll
      *     epoll create
      *
      * */
    epfd = epoll_create(2);
    if (epfd < 0) {
        perror("epoll_create()");
        exit(EXIT_FAILURE);
    }

    memset(&event, 0, sizeof(struct epoll_event));
    events = malloc(sizeof(struct epoll_event) * 2);
    event.data.fd = fd1;
    event.events = EPOLLIN | EPOLLOUT;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd1, &event);

    event.data.fd = fd2;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd2, &event);

    while (stat12.status != STAT_T || stat21.status != STAT_T) {
        /*
        event.events = 0;
        if (stat12.status == STAT_R)
            event.events |= EPOLLIN;
        if (stat21.status == STAT_W)
            event.events |= EPOLLOUT;
        epoll_ctl(epfd, EPOLL_CTL_MOD, fd1, &event);

        if (stat21.status == STAT_R)
        if (stat21.status == STAT_W)

        ....
         * */
        if (stat12.status < STAT_AUTO || stat21.status < STAT_AUTO) {
            while (epoll_wait(epfd, events, 2, -1) < 0) {
                if (errno == EINTR)
                    continue;
                perror("epoll_wait()");
                exit(EXIT_FAILURE);
            }
        }

        if ((events[0].data.fd == fd1 && events[0].events & EPOLLIN)
                || (events[0].data.fd == fd2 && events[0].events & EPOLLOUT)
                || stat12.status > STAT_AUTO)
            state_drive(&stat12);
        if ((events[1].data.fd == fd2 && events[1].events & EPOLLIN)
                || (events[1].data.fd == fd1 && events[1].events & EPOLLOUT)
                || stat12.status > STAT_AUTO)
            state_drive(&stat21);
    }
    fcntl(fd1, F_SETFL, save_fd1);
    fcntl(fd2, F_SETFL, save_fd2);
    close(epfd);
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
