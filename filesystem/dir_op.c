#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>


int main(int argc, char **argv)
{
    DIR *dp;
    struct dirent *dirinfo;

    if (argc < 2) {
        fprintf(stderr, "Usage: <%s> <dir...>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (NULL == (dp = opendir(argv[1]))) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while (1) {
        dirinfo = readdir(dp);
        if (dirinfo == NULL)
            break;

        printf("%s\n", dirinfo->d_name);
    }

    closedir(dp);
}
