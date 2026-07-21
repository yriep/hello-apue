#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(void)
{
    int fd1, fd2;
    char str[10] = {0};

    fd1 = open("./opentwo", O_CREAT | O_RDONLY);
    if (fd1 < 0)
        exit(1);

    fd2 = open("./opentwo", O_TRUNC | O_WRONLY);
    if (fd2 < 0)
        exit(1);

    //for (int i = 0; i < 10; i++) {
        write(fd2, "test", sizeof(char) * 5);
    //}

    close(fd2);
    char msg[10];
    read(fd1, msg, sizeof(char) * 5);
    printf("get %s\n", msg);
    close(fd1);
    return 0;
}
