#include <stdio.h>
#include <stdlib.h>
#include <error.h>

int main(int argc, char *argv[])
{
    FILE *fp;
    long n;

    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("fopen()");
        exit(-1);
    }
    fseek(fp, 0, SEEK_END);
    n = ftell(fp);
    printf("file len is %ld\n", n);

    fclose(fp);
    return 0;
}
