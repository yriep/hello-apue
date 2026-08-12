#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <error.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define FNAME   "/tmp/out"

static void add_func(int fd)
{
    FILE *fp;
    char num_s[32] = {0};

    fp = fdopen(fd, "r+");
    if (fp == NULL) {
        perror("fdopen()");
        exit(EXIT_FAILURE);
    }

    lockf(fd, F_LOCK, 0);

//    fseek(fp, 0, SEEK_SET);     //多进程的文件描述符都是由fork复制而来，指向的都是同一个结构体，偏移量也会共用。
    fgets(num_s, 32 - 1, fp);

    fseek(fp, 0, SEEK_SET);
    fprintf(fp, "%d", atoi(num_s) + 1);
    fflush(fp);

    fseek(fp, 0, SEEK_SET);     //多进程的文件描述符都是由fork复制而来，指向的都是同一个结构体，偏移量也会共用。
    //如果是子进程单独打开文件时，就是每个文件描述符指向不同的结构体了。
    lockf(fd, F_ULOCK, 0);

}

int main(void)
{
    int i = 0;
    int fd;
    char num[32];
    pid_t pid;

    fd = open(FNAME, O_RDWR);
    if (fd < 0) {
        perror("open()");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < 20; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork()");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            add_func(fd);
            exit(EXIT_SUCCESS);
        }
    }

    for (i = 0; i < 20; i++) {
        wait(NULL);
    }
//    lseek(fd, 0, SEEK_SET);
    read(fd, num, 31);
    puts(num);
    close(fd);
    return 0;
}
