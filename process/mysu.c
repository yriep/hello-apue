/*
 * mysu 0（root） cat /etc/shadow
 *
 * 使用输入用户信息执行命令
 * 
 * 使用setuid，更改effective uid 执行二进制文件 bh
 * */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        perror("fork()");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        wait(NULL);
        exit(0);
    }
//    setuid(atoi(argv[1]));
//    setuid 一般只能由root用户调用，临时取消root权限的。非root用户只能设置成user id或saved set id。此时其实用不用都行，因为文件是u+s权限
//    本来就是以root权限运行。  
    if (-1 == execvp(argv[2], argv+2)) {
        perror("execvp()");
    }
    return 0;
}

