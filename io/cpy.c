#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    FILE *fps, *fpd;
    int ch;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <file source> <file d>", argv[0]);
        exit(EXIT_FAILURE);
    }
    fps = fopen(argv[1], "r");
    if (fps == NULL) {
        perror("fopen()");
        exit(EXIT_FAILURE);
    }

    fpd = fopen(argv[2], "w");
    if (fpd == NULL) {
        fclose(fps);
        perror("fopen()");
        exit(EXIT_FAILURE);
    }

    while (1) {
        if ((ch = fgetc(fps)) == EOF)
            break;
        fputc(ch, fpd);
    }
    fclose(fps);
    fclose(fpd);
}
