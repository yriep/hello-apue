#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

// 令牌桶 闲时攒权限，忙时根据权限进行
#define BURST       100
#define CPS         10
#define BUF_SIZE    CPS

static volatile sig_atomic_t tokens = 0;

static void alrm_handler(int sig)
{
    alarm(1);
    if (sig == SIGALRM && tokens <= BURST) {
        tokens++;
    }
}

int main(int argc, char *argv[])
{
    int fd;
    int str_len = 0;
    int ret = 0;
    char buf[BUF_SIZE];

    if (argc < 2) {
        fprintf(stderr, "Usage <%s> <F>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    signal(SIGALRM, alrm_handler);
    alarm(1);

    while ((fd = open(argv[1], O_RDONLY)) < 0) {
        if (errno != EINTR) {
            perror("open()");
            exit(EXIT_FAILURE);
        }

    }

    do {
        if (tokens <= 0)
            pause();
        tokens--;
        //当信号处理函数的token++和此处的token--同时发生；需要保证原子操作token设为sig_atomic_t类型

        while((str_len = read(fd, buf, BUF_SIZE)) < 0) {
            if (errno == EINTR)
                continue;
            perror("read()");
            exit(EXIT_FAILURE);
        }

        if (str_len == 0)
            break;

        int str_pos = 0;
        while(str_len > 0) {
            ret = write(1, buf + str_pos, str_len);
            if (ret < 0) {
                if (errno == EINTR)
                    continue;

                perror("write()");
                exit(EXIT_FAILURE);
            }
            str_pos += ret;
            str_len -= ret;
        }
    } while (1);

    close(fd);
    return 0;
}
