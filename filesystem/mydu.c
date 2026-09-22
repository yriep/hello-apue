#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <glob.h>

#define PATHSIZE 1024

// lstat 会计算硬链接指向文件的大小，而du会忽略硬链接文件大小。每有一个硬链接，都会多4K
static bool path_noloop(const char *path)
{
    char *root;

    root = strrchr(path, '/');
    if (root == NULL)
        exit(1);
    if (strcmp(root + 1, ".") == 0 || strcmp(root + 1, "..") == 0)
        return false;
    return true;
}
// path: /AAAA/BBBB/CCC....
static int64_t mydu(const char *path)
{
    int64_t sum = 0;
    char pat[PATHSIZE];
    struct stat statbuf;
    glob_t globbuf;

    if (0 > lstat(path, &statbuf)) {
        perror("lstat");
        exit(EXIT_FAILURE);
    }

    if (!S_ISDIR(statbuf.st_mode)) {
        return statbuf.st_blocks;
    }

    strncpy(pat, path, PATHSIZE);
    strncat(pat, "/*", PATHSIZE - strlen("/*") - 1);
    glob(pat, 0, NULL, &globbuf);

    strncpy(pat, path, PATHSIZE);
    strncat(pat, "/.*", PATHSIZE - strlen("/.*") - 1);
    glob(pat, GLOB_APPEND, NULL, &globbuf);

    sum += statbuf.st_blocks;
    for (int i = 0; i < globbuf.gl_pathc; i++) {
        if (path_noloop(globbuf.gl_pathv[i]))
            sum += mydu(globbuf.gl_pathv[i]);
    }
    return sum;
}


int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: <%s> <path...>\n", argv[0]);
    }

    for (int i = 1; i < argc; i++) {
        printf("%ld\n", mydu(argv[i]) / 2);
    }
    return 0;
}
