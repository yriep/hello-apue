#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#include "mypipe.h"

#define THRNUM  4
#define LEFT    30000000
#define RIGHT   30000200

static pipe *pip;

static void *thr_primer(void *p)
{
    int num, len, ret, pos = 0;;
    int isprimer;
    char ch[len];

    pipe_register(pip, READER);

    /*
     * 1. read 中要获取4字节的char数组，再转换成int类型。
     * 2. write 中要将int转换成char数组的字节形式，写入队列中
     */

    while (1) {
        pos = 0;
        len = sizeof(int);
        while (len > 0) {
            ret = pipe_read(pip, ch+pos, len);
            if (ret < 0) {
                fprintf(stderr, "pipe_read err...");
                pthread_exit(NULL);
            }
            len -= ret;
            pos += ret;
        }
        memcpy(&num, ch, sizeof(int));

        // printf("[%d] get num is %d\n", (int)p, num);
        isprimer = 1;
        for (int i = 2; i < num / 2; i++) {
            if (num % i == 0) {
                isprimer = 0;
                break;
            }
        }

        if (isprimer) {
            printf("[pid %d]: %d is a primer!\n", (int)p, num);
        }
    }

    return NULL;
}

int main(void)
{
    int i, ret, len, pos = 0;
    pthread_t pid[THRNUM];
    int size = sizeof(int);
    char intstr[size];

    pip = pipe_init();

    for (i = 0; i < THRNUM; i++) {
        pthread_create(&pid[i], NULL, thr_primer, (void *)i);
    }

    pipe_register(pip, WRITER);

    for (i = LEFT; i <= RIGHT; i++) {
        memcpy(intstr, &i, sizeof(int));
        len = sizeof(int);
        pos = 0;
        while (len > 0)
        {
            ret = pipe_write(pip, intstr+pos, len);
            if (ret < 0) {
                fprintf(stderr, "pipe_write err..");
                exit(EXIT_FAILURE);
            }
            len -= ret;
            pos += ret;
        }
    }

    pipe_unregister(pip, WRITER);
    for (i = 0; i < THRNUM; i++) {
        pthread_join(pid[i], NULL);
    }
    pipe_destroy(pip);
    return 0;
}
