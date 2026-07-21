#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char *argv[])
{
     if (argc > 2) {
         fprintf(stderr, "Usage: <source file> <des file>");
         exit(EXIT_FAILURE);
     }

    fp1 = fopen();
    fp2 = fopen();

    fgetc();
    fputc();


    fclose();
    fclose();
    return 0;
}
