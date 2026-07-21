#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char *linebuf;
    size_t linesize;
    FILE * fp;

    if (argc < 2) {
        fprintf(stderr, "Usage: <> <>");
        exit(EXIT_FAILURE);
    }

    fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        perror("fopen()");
        exit(EXIT_FAILURE);
    }

    linebuf = NULL;
    linesize = 0;
    while (1) {
        if (0 > getline(&linebuf, &linesize, fp))
            break;

        printf("line strlen: %zd\n", strlen(linebuf));
        printf("line size: %zd\n", linesize);
    }

    fclose(fp);
    return 0;
}
