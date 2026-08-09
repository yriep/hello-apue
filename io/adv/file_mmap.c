#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
    int fd;
    char *ptr;
    int count = 0;
    struct stat filestat;

    if (argc < 2) {
        fprintf(stderr, "Usage: <%s> <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open()");
        exit(EXIT_FAILURE);
    }

    if (fstat(fd, &filestat) < 0) {
        perror("fstat()");
        close(fd);
        exit(EXIT_FAILURE);
    }

    ptr = mmap(NULL, filestat.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap()");
        close(fd);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < filestat.st_size; i++) {
        if (ptr[i] == '#')
            count++;
    }
    printf("get # number is %d\n", count);

    close(fd);
    munmap(ptr, filestat.st_size);
    return 0;
}
