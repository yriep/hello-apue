// 指定n个进程
/*
分块法：
300000----1----｜---2---｜---3---300200

1号负载最重：数据越小，质数越多

交叉分配法：
取余

此模型依旧不能平均分配负载

但是普通情况下，交叉分配大部分情况下较好

池内算法：竞争和冲突 -- 原子操作
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define LEFT  30000000
#define RIGHT 30000200
#define N     3

int main(void)
{
    int n = 0;
    int i, j, mark;
    pid_t pid;

    for (n = 0 ; n < N; n++) {          //分块法
        pid = fork();
        if (pid < 0) {
            perror("fork");  //失败退出时，应该循环等待回收子进程资源。
            exit(1);
        }
        if (pid == 0) {
            for (i = LEFT + n; i <= RIGHT; i += N) {
                mark = 1;
                for (j = 2; j < i / 2; j++) {
                    if (i % j == 0) {
                        mark = 0;
                        break;
                    }
                }

                if (mark)
                    printf("[%d]: %d is a primer\n", n, i);

            }
            exit(0); //不加exit会导致子进程下一个循环fork。
        }
    }
     
    for (n = 0; n < N; n++) {
        pid = wait(NULL);
    }
    exit(0);
}
